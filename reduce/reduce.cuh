/***
  *  Project:
  *
  *  File: reduce.cuh
  *  Created: Feb 12, 2013
  *  Modified: Wed 13 Feb 2013 01:44:32 PM PST
  *
  *  Author: Abhinav Sarje <asarje@lbl.gov>
  */


namespace woo {
namespace cuda {

	const unsigned int MAX_CUDA_THREADS_ = 1024;
	const unsigned int GRID_SIZE_ = 65535;	// max number of blocks
	const unsigned int BLOCK_DIM_ = 256;	// number of threads - assuming power of 2
	const unsigned int NUM_SUBTILES_ = 8;	// number of subtiles in a block (processed by one thread block)

	extern __shared__ unsigned char d_data_sh[];


	template <typename data_t, typename functor_t>
	__global__
	void reduce_block(data_t *input, unsigned int n, data_t init, functor_t op,
						data_t* output, unsigned int num_subtiles) {
		unsigned int output_i = blockIdx.x;
		unsigned int input_i = num_subtiles * blockDim.x * blockIdx.x + threadIdx.x;

		data_t *data_sh = (data_t*) d_data_sh;

		// start with init
		data_t sum = init;
		if(input_i < n) sum = input[input_i];

		// reduce the subtiles into single set (one sum for each thread)
		// this is the sequential component
		for(unsigned int i = 1; i < num_subtiles; ++ i) {
			input_i += blockDim.x;
			if(input_i < n) sum = op(sum, input[input_i]);
		} // for

		// latter half threads store their data to shared memory
		// first half reduce those to their registers
		// reduce in log_2(blockDim.x) steps
		unsigned int len = blockDim.x >> 1;
		while(len > 0) {
			if(threadIdx.x >= len && threadIdx.x < len << 1) data_sh[threadIdx.x] = sum;
			__syncthreads();
			if(threadIdx.x < len) sum = op(sum, data_sh[len + threadIdx.x]);
			len = len >> 1;
		} // while

		// write reduced output to global memory
		if(threadIdx.x == 0) output[output_i] = sum;
	} // reduce_block()


	// this performs better than thrust::reduce
	template <typename iterator_t, typename data_t, typename functor_t>
	data_t reduce(iterator_t start, iterator_t end, data_t init, functor_t op) {

		unsigned int num_subtiles = NUM_SUBTILES_;
		unsigned int n = end - start;
		unsigned int block_size = BLOCK_DIM_;
		unsigned int n_grid_max = GRID_SIZE_ * block_size * num_subtiles;
		unsigned int num_grids = ceil((float) n / n_grid_max);

		data_t *output = NULL, *base_output = NULL;
		unsigned int output_size = GRID_SIZE_;
		cudaMalloc((void**) &output, output_size * sizeof(data_t));
		base_output = output;

		unsigned int d_shmem_size = block_size * sizeof(data_t);

		//std::cout << "num_grids: " << num_grids << ", n: " << n
		//			<< ", d_shmem_size: " << d_shmem_size << std::endl;

		data_t result = init;
		for(unsigned int grid = 0; grid < num_grids; ++ grid) {	// for each grid
			// number of data elements to process in this grid:
			unsigned int n_grid = (grid == num_grids - 1) ?
									n - (num_grids - 1) * n_grid_max : n_grid_max;
			// the input data segment to process
			data_t *input = start + grid * n_grid_max;
			output = base_output;

			while(1) {
				unsigned int num_blocks = ceil((float) n_grid / (block_size * num_subtiles));
				//std::cout << grid << ". num_blocks: " << num_blocks << ", block_size: " << block_size
				//			<< ", n_grid: " << n_grid << std::endl;
				reduce_block <<< num_blocks, block_size, d_shmem_size >>>
								(input, n_grid, init, op, output, num_subtiles);
				cudaError_t err = cudaGetLastError();
				if(err != cudaSuccess) {
					std::cerr << "error: something went wrong in kernel launch: "
								<< cudaGetErrorString(err) << std::endl;
				} // if
				cudaThreadSynchronize();
				if(num_blocks == 1) break;
				n_grid = num_blocks;
				data_t *temp = input; input = output; output = temp;
			} // while

			data_t temp_result;
			cudaMemcpy(&temp_result, &output[0], sizeof(data_t), cudaMemcpyDeviceToHost);
			result = op(result, temp_result);
			//std::cout << grid << ". result: " << result << std::endl;
		} // for

		cudaFree(output);

		return result;
	} // reduce()


	template <typename data_t, typename functor_t>
	__global__
	void reduce_block_old(data_t* array, unsigned int n, data_t init, functor_t op, data_t* output) {
		unsigned int output_i = blockIdx.x;
		unsigned int input_i = blockDim.x * blockIdx.x + threadIdx.x;

		data_t *data_sh = (data_t*) d_data_sh;

		// load input
		if(input_i < n) data_sh[threadIdx.x] = array[input_i];
		else data_sh[threadIdx.x] = init;	// padding
		__syncthreads();

		// reduce in log_2(MAX_CUDA_THREADS_) steps
		unsigned int len = blockDim.x;
		while(len > 0) {
			len = len >> 1;
			if(threadIdx.x < len) data_sh[threadIdx.x] = op(data_sh[threadIdx.x], data_sh[threadIdx.x + len]);
			__syncthreads();
		} // while

		// write reduced output
		if(threadIdx.x == 0) output[output_i] = data_sh[0];
	} // reduce_block()


	template <typename iterator_t, typename data_t, typename functor_t>
	data_t reduce_old(iterator_t start, iterator_t end, data_t init, functor_t op) {

		// assuming all data is already on the device

		unsigned int n = end - start;
		unsigned int block_size = MAX_CUDA_THREADS_;
		unsigned int num_blocks = ceil(n / (float)block_size);

		data_t *base_input = NULL, *output = NULL, *temp = NULL;
		base_input = (data_t*) start;

		data_t *data_mem = NULL;
		unsigned int num_iter = ceil((float)num_blocks / 65535);
		unsigned int out_size = (num_iter > 1) ? 65535 : num_blocks;
		cudaMalloc((void**) &data_mem, out_size * sizeof(data_t));

		size_t d_shmem_size = MAX_CUDA_THREADS_ * sizeof(data_t);

		data_t result = init, temp_result = init;
		for(unsigned int i = 0; i < num_iter; ++ i) {
			unsigned int n_kernel = (num_iter > 1) ? 65535 * block_size : n;
			data_t* input = base_input + i * n_kernel;
			output = data_mem;
			if(i == num_iter - 1) {
				n_kernel = n % (65535 * block_size);
				num_blocks = ceil(n_kernel / (float)block_size);
			} else { num_blocks = 65535; }
			//woo::BoostChronoTimer mytimer;
			while(1) {
				//mytimer.start();
				reduce_block_old <<< num_blocks, block_size, d_shmem_size >>>
									(input, n_kernel, init, op, output);
				cudaThreadSynchronize();
				//mytimer.stop();
				//std::cout << "this time (" << n_kernel << "): " << mytimer.elapsed_msec() << std::endl;
				if(num_blocks == 1) break;
				// output becomes input and vice versa
				temp = output; output = input; input = temp;
				n_kernel = num_blocks;
				num_blocks = ceil(n_kernel / (float)block_size);
			} // while
			cudaMemcpy(&temp_result, &output[0], sizeof(data_t), cudaMemcpyDeviceToHost);
			result = op(result, temp_result);
		} // for

		cudaFree(data_mem);
		return result;
	} // reduce()


} // namespace cuda
} // namespace woo
