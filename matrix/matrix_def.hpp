/***
  *  Project: Woo Matrix Library
  *
  *  File: matrix_def.hpp
  *  Created: Dec 03, 2012
  *  Modified: Wed 05 Dec 2012 03:25:48 PM PST
  *
  *  Author: Abhinav Sarje <asarje@lbl.gov>
  */

#include <vector>
#include <cstring>
#include <iostream>

#ifndef _MATRIX_DEF_HPP_
#define _MATRIX_DEF_HPP_

namespace woo {

	template <typename value_type> class ColumnIterator;
	template <typename value_type> class RowIterator;

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
			col_iterator begin_col() {
				col_iterator start_col(0, num_rows_, num_cols_, this);
				return start_col;
			} // begin()

			// iterator to column after last (end_index)
			col_iterator end_col() {
				col_iterator last_col(num_cols_, num_rows_, num_cols_, this);
				return last_col;
			} // end()


			// iterator to row
			row_iterator row(unsigned int i) {
				if(i <= 0) {
					row_iterator start_row(0, num_cols_, num_rows_, this);
					return start_row;
				} // if
				if(i >= num_rows_) {
					row_iterator end_row(num_rows_ - 1, num_cols_, num_rows_, this);
					return end_row;
				} // if
				row_iterator row(i, num_cols_, num_cols_, this);
				return row;
			} // column()

			// iterator to first row
			row_iterator begin_row() {
				row_iterator start_row(0, num_cols_, num_rows_, this);
				return start_row;
			} // begin()

			// iterator to row after last (end_index)
			row_iterator end_row() {
				row_iterator last_row(num_rows_, num_cols_, num_rows_, this);
				return last_row;
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

} // namespace woo

#endif // _MATRIX_DEF_HPP_
