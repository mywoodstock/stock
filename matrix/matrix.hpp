/***
  *  Project: Woo Matrix Library
  *
  *  File: matrix.hpp
  *  Created: Dec 03, 2012
  *  Modified: Tue 04 Dec 2012 05:52:01 PM PST
  *
  *  Author: Abhinav Sarje <asarje@lbl.gov>
  */

#include <vector>
#include <cstring>
#include <iostream>

namespace woo {

	template <typename value_type> class ColumnIterator;
	template <typename value_type> class RowIterator;

//	typedef ColumnIterator col_iterator;
//	typedef RowIterator row_iterator;
	// how to define const iterators ...
	//typedef const ColumnIterator const_col_iterator;
	//typedef const RowIterator const_row_iterator;

	template <typename value_type>
	class Matrix {
		protected:
			value_type *mat_;
			unsigned int num_dims_;				// number of dimensions
			std::vector<unsigned int> dims_;	// values of dimensions

		public:
			// default constructor: matrix size not known
			Matrix(): mat_(NULL), num_dims_(0) {
			} // Matrix()


			// generic constructor
			Matrix(unsigned int num_dims, const std::vector<unsigned int>& dims):
					mat_(NULL), num_dims_(num_dims) {
				if(dims.size() != num_dims) {
					std::cerr << "error: number of dimensions does not match list of dimension values"
								<< std::endl;
					return;
				} // if
				dims_.clear();
				unsigned int total_elems = 1;
				for(unsigned int i = 0; i < num_dims; ++ i) {
					dims_.push_back(dims[i]);
					total_elems *= dims[i];
				} // for
				mat_ = new (std::nothrow) value_type[total_elems];
				if(mat_ == NULL) {
					std::cerr << "error: failed to allocate memory for the matrix" << std::endl;
					return;
				} // if
				memset(mat_, 0, total_elems * sizeof(value_type));
			} // Matrix()


			// constructor for 2D
			Matrix(unsigned int rows, unsigned int cols):
					mat_(NULL), num_dims_(2) {
				std::vector<unsigned int> dims;
				dims.push_back(rows);
				dims.push_back(cols);
				Matrix(2, dims);
			} // Matrix()


			~Matrix() {
				if(mat_ != NULL) delete[] mat_;
			} // ~Matrix()


			// a few accessors
			unsigned int dims() const { return num_dims_; }
			unsigned int dim_size(unsigned int i) const { return dims_[i]; }


	}; // class Matrix


	template <typename value_type>
	class Matrix2D : public Matrix <value_type> {
		public:

			typedef ColumnIterator<value_type> col_iterator;
			typedef RowIterator<value_type> row_iterator;

			friend class ColumnIterator<value_type>;
			friend class RowIterator<value_type>;

			class IndexType {
				public:
					IndexType(): num_(0), idx_(0) { }
					IndexType(int a, int b): num_(a), idx_(b) { }
					~IndexType() { }

					int num_;		// column/row number
					int idx_;		// index within the column/row
			}; // class IndexType

			typedef IndexType index_type;
			static const index_type end_index;
			static const index_type begin_index;


			// constructor
			Matrix2D(unsigned int rows, unsigned int cols):
				Matrix<value_type>(rows, cols) { //, num_cols_(cols), num_rows_(rows) {
				num_cols_ = cols;
				num_rows_ = rows;
			} // Matrix()


			// iterator to column
			col_iterator column(unsigned int i) {
				if(num_cols_ <= 0) {
					col_iterator start_col(0, num_rows_);
					return start_col;
				} // if
				if(i >= num_cols_) {
					col_iterator end_col(num_cols_ - 1, num_rows_);
					return end_col;
				} // if
				col_iterator col(i, num_rows_);
				return col;
			} // column()


			// accessors

			unsigned int num_cols() const { return num_cols_; }
			unsigned int num_rows() const { return num_rows_; }

			// access (i, j)-th element
			value_type& operator()(unsigned int i, unsigned int j) {
				return Matrix<value_type>::mat_[num_cols_ * i + j];
			} // operator()()


			// insert stuff
			bool insert_row(unsigned int i, const std::vector<value_type>& row) { }
			bool insert_col(unsigned int i, const std::vector<value_type>& col) { }

		private:
			unsigned int num_cols_;		// number of columns = row size
			unsigned int num_rows_;		// number of rows = col size

	}; // class Matrix2D

	// begin and end index_type constants
	template<> const Matrix2D<float>::index_type Matrix2D<float>::begin_index(0, 0);
	template<> const Matrix2D<float>::index_type Matrix2D<float>::end_index(-1, -1);
	template<> const Matrix2D<double>::index_type Matrix2D<double>::begin_index(0, 0);
	template<> const Matrix2D<double>::index_type Matrix2D<double>::end_index(-1, -1);
	template<> const Matrix2D<int>::index_type Matrix2D<int>::begin_index(0, 0);
	template<> const Matrix2D<int>::index_type Matrix2D<int>::end_index(-1, -1);
	template<> const Matrix2D<unsigned int>::index_type Matrix2D<unsigned int>::begin_index(0, 0);
	template<> const Matrix2D<unsigned int>::index_type Matrix2D<unsigned int>::end_index(-1, -1);
	template<> const Matrix2D<long int>::index_type Matrix2D<long int>::begin_index(0, 0);
	template<> const Matrix2D<long int>::index_type Matrix2D<long int>::end_index(-1, -1);
	template<> const Matrix2D<unsigned long int>::index_type Matrix2D<unsigned long int>::begin_index(0, 0);
	template<> const Matrix2D<unsigned long int>::index_type Matrix2D<unsigned long int>::end_index(-1, -1);


	/* a generic iterator */
	template <typename value_type>
	class DimensionIterator {
		public:

			DimensionIterator() { DimensionIterator(0, 0); }
			DimensionIterator(unsigned int d, unsigned int size):
				dim_num_(d), dim_size_(size) { }
			~DimensionIterator() { }

			virtual void operator++() { }
			virtual void operator--() { }

		protected:

			value_type* dim_pointer_;		// pointer to an index
			unsigned int dim_num_;			// dimension number
			unsigned int dim_size_;			// size of the dimension

	}; // class DimensionIterator


	/* column vector for Matrix2D */
	template <typename value_type>
	class ColumnIterator : public DimensionIterator <value_type> {
		public:

			/* create an iterator pointing to column 0 */
			ColumnIterator() {
				DimensionIterator<value_type>(1, 0);
				index_.num_ = 0;
				index_.idx_ = 0;
			} // ColumnIterator()

			ColumnIterator(unsigned int i, unsigned int size) {
				DimensionIterator<value_type>(i, size);
				index_.num_ = i;
				index_.idx_ = 0;
			} // ColumnIterator()

			/* increment to next column */
			void operator++() {		// next column
				if(index_.num_ == parent_mat_->num_rows_ - 1) {
					index_ = Matrix2D<value_type>::end_index;
					this->dim_pointer_ = NULL;
				} else {
					++ index_.num_;
					index_.idx_ = 0;
					unsigned int index = index_.idx_ * parent_mat_->num_cols_ + index_.num_;
					this->dim_pointer_ = &(parent_mat_->mat_[index]);
				} // if-else
			} // operator++()

			/* decrement to previous column */
			void operator--() {		// previous column
				if(index_.num_ != 0) {
					-- index_.num_;
					index_.idx_ = 0;
					unsigned int index = index_.idx_ * parent_mat_->num_cols_ + index_.num_;
					this->dim_pointer_ = &(parent_mat_->mat_[index]);
				} // if-else
			} // operator--()

			/* assign a pointer to the iterator */
			void operator=(value_type* pointer) {
				this->dim_pointer_ = pointer;
				// what about index_ ?
			} // operator=()

			/* return the i-th element of current column */
			value_type& operator[](unsigned int i) {
				if(i >= parent_mat_->num_rows_) return NULL;
				index_.idx_ = i;
				return parent_mat_->mat_[index_.idx_ * parent_mat_->num_cols_ + index_.num_];
			} // operator[]()

			/* return the value of current column's current index */
			value_type value() {
				unsigned int index = index_.idx_ * parent_mat_->num_cols_ + index_.num_;
				return parent_mat_->mat_[index];
			} // value()

		private:
			Matrix2D<value_type>* parent_mat_;					// is iterator of this object
			typename Matrix2D<value_type>::index_type index_;	// index in matrix

	}; // class ColumnIterator


	/* row iterator for Matrix2D */
	template <typename value_type>
	class RowIterator : public Matrix<value_type>::DimensionIterator {
		public:
			RowIterator() {
			} // RowIterator()

			void operator++() {
			} // operator++()

			void operator--() {
			} // operator--()

			void operator=(value_type* pointer) {
			} // operator=()
	}; // class ColumnIterator

} // namespace woo
