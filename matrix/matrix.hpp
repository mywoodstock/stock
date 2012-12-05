/***
  *  Project: Woo Matrix Library
  *
  *  File: matrix.hpp
  *  Created: Dec 03, 2012
  *  Modified: Wed 05 Dec 2012 11:33:45 AM PST
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
			unsigned int capacity_;

		public:
			// default constructor: matrix size not known
			Matrix(): mat_(NULL), num_dims_(0) {
			} // Matrix()


			// generic constructor
			//Matrix(unsigned int num_dims, const std::vector<unsigned int>& dims):
			Matrix(unsigned int num_dims):
					mat_(NULL), num_dims_(num_dims), capacity_(0) {
			} // Matrix()


			void init(const std::vector<unsigned int>& dims) {
				if(dims.size() != num_dims_) {
					std::cerr << "error: number of dimensions does not match list of dimension values"
								<< std::endl;
					return;
				} // if
				dims_.clear();
				unsigned int total_elems = 1;
				for(unsigned int i = 0; i < num_dims_; ++ i) {
					dims_.push_back(dims[i]);
					total_elems *= dims[i];
				} // for
				unsigned int size = 256;
				while(true) {
					if(total_elems <= size) {
						total_elems = size;
						break;
					} // if
					size *= 2;
				} // while
				capacity_ = total_elems;
				mat_ = new (std::nothrow) value_type[total_elems];
				if(mat_ == NULL) {
					std::cerr << "error: failed to allocate memory for the matrix" << std::endl;
					return;
				} // if
				memset(mat_, 0, total_elems * sizeof(value_type));
			} // init()


			// destructor
			~Matrix() {
				if(mat_ != NULL) delete[] mat_;
			} // ~Matrix()


			// a few accessors
			unsigned int dims() const { return num_dims_; }
			unsigned int dim_size(unsigned int i) const { return dims_[i]; }
			unsigned int capacity() const { return capacity_; }

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

					bool operator==(IndexType other) {
						return (other.num_ == num_ && other.idx_ == idx_);
					} // operator==()

					int num_;		// column/row number
					int idx_;		// index within the column/row
			}; // class IndexType

			typedef IndexType index_type;
			static const index_type end_index;
			static const index_type begin_index;


			// constructor
			Matrix2D(unsigned int rows, unsigned int cols):
				Matrix<value_type>(2) {
				num_rows_ = rows;
				num_cols_ = cols;
				std::vector<unsigned int> dims;
				dims.push_back(num_rows_);
				dims.push_back(num_cols_);
				this->init(dims);
			} // Matrix()


			// iterator to column
			col_iterator column(unsigned int i) {
				if(i <= 0) {
					col_iterator start_col(0, num_rows_, num_cols_, this);
					return start_col;
				} // if
				if(i >= num_cols_) {
					col_iterator end_col(num_cols_ - 1, num_rows_, num_cols_, this);
					return end_col;
				} // if
				col_iterator col(i, num_rows_, num_cols_, this);
				return col;
			} // column()

			// iterator to first column
			col_iterator begin() {
				col_iterator start_col(0, num_rows_, num_cols_, this);
				return start_col;
			} // begin()

			// iterator to column after last (end_index)
			col_iterator end() {
				col_iterator end_col(num_cols_, num_rows_, num_cols_, this);
				return end_col;
			} // end()


			// accessors

			unsigned int num_cols() const { return num_cols_; }
			unsigned int num_rows() const { return num_rows_; }
			unsigned int size() const { return num_cols_ * num_rows_; }

			// access (i, j)-th element
			value_type& operator()(unsigned int i, unsigned int j) {
				return this->mat_[num_cols_ * i + j];
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

			//DimensionIterator() { DimensionIterator(0, 0, 0, NULL); }
			DimensionIterator(unsigned int d, unsigned int size, unsigned int i, value_type* mat):
				dim_num_(d), dim_size_(size), dim_index_(i), dim_pointer_(mat) { }
			~DimensionIterator() { }

			virtual void operator++() { }
			virtual void operator--() { }

		protected:

			unsigned int dim_num_;			// dimension number
			unsigned int dim_size_;			// size of the dimension
			unsigned int dim_index_;		// index of the current dimension vector
			value_type* dim_pointer_;		// pointer to current dimension vector

	}; // class DimensionIterator


	/* column vector for Matrix2D */
	template <typename value_type>
	class ColumnIterator : public DimensionIterator <value_type> {
		private:
			Matrix2D<value_type>* parent_mat_;					// is iterator of this object
			typename Matrix2D<value_type>::index_type index_;	// index in matrix

		public:
			/* create an iterator pointing to column 0 */
			ColumnIterator() {
				DimensionIterator<value_type>(1, 0);
				index_.num_ = 0;
				index_.idx_ = 0;
			} // ColumnIterator()

			ColumnIterator(unsigned int i, unsigned int num_rows, unsigned int num_cols,
							Matrix2D<value_type>* mat): 
				DimensionIterator<value_type>(1, num_rows, i, mat->mat_) {
				parent_mat_ = mat;
				if(i >= num_cols) {
					index_ = Matrix2D<value_type>::end_index;
				} else {
					index_.num_ = i;
					index_.idx_ = 0;
				} // if-else
			} // ColumnIterator()

			/* increment to next column */
			void operator++() {		// next column
				if(index_ == Matrix2D<value_type>::end_index) {
					// do nothing
				} else if(index_.num_ == parent_mat_->num_rows_ - 1) {
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
				if(index_ == Matrix2D<value_type>::end_index) {
					index_.num_ = parent_mat_->num_cols_ - 1;
				} else if(index_.num_ == 0) {
					index_ = Matrix2D<value_type>::begin_index;
				} else if(index_.num_ != 0) {
					-- index_.num_;
				} // if-else
				index_.idx_ = 0;
				unsigned int index = index_.idx_ * parent_mat_->num_cols_ + index_.num_;
				this->dim_pointer_ = &(parent_mat_->mat_[index]);
			} // operator--()

			/* assign a pointer to the iterator */
			void operator=(value_type* pointer) {
				this->dim_pointer_ = pointer;
				// what about index_ ?
			} // operator=()

			/* return the i-th element of current column */
			value_type& operator[](unsigned int i) {
				if(i >= parent_mat_->num_rows_) {
					return parent_mat_->mat_[(parent_mat_->num_rows_ - 1) * parent_mat_->num_cols_ +
												parent_mat_->num_rows_ - 1];
				} // if
			//	index_.idx_ = i;
				return parent_mat_->mat_[i * parent_mat_->num_cols_ + index_.num_];
			} // operator[]()

			/* comparison operators */

			bool operator==(ColumnIterator other) {
				return (other.index_ == index_ && other.parent_mat_ == parent_mat_);
			} // operator==()

			bool operator!=(ColumnIterator other) {
				return !(other.index_ == index_ && other.parent_mat_ == parent_mat_);
			} // operator!=()

			/* return the value of current column's current index */
			value_type value() const {
				unsigned int index = index_.idx_ * parent_mat_->num_cols_ + index_.num_;
				return parent_mat_->mat_[index];
			} // value()

			unsigned int size() const { return parent_mat_->num_rows_; }

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
