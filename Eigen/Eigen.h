// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Eigen_Eigen_h
#define _Eigen_Eigen_h

#define EIGEN_MATRIX_PLUGIN     <drx/Eigen/ToStringPlugin.h>
#define EIGEN_DENSEBASE_PLUGIN  <drx/Eigen/ToStringPlugin.h>
#define EIGEN_TENSOR_PLUGIN     <drx/Eigen/ToStringPlugin.h>

#define EIGEN_MPL2_ONLY

#ifndef _DEBUG
#define EIGEN_NO_DEBUG
#else
#define EIGEN_INITIALIZE_MATRICES_BY_NAN
#endif

#define eigen_assert(x) ASSERT(x)

#undef Success
#include <drx/Eigen/Dense>
#include <drx/Eigen/NonLinearOptimization>
#undef Complex
#include <drx/Eigen/FFT>
#include <drx/Eigen/CXX11/Tensor>


namespace drx {

template <class T>
using UVector = drx::Vec<T>;

template <class T>
using UArray = drx::Array<T>;

template <class T>
using UIndex = drx::Index<T>;

template<typename _Scalar, ptrdiff_t nx = Eigen::Dynamic, ptrdiff_t ny = Eigen::Dynamic>
struct NonLinearOptimizationFunctor {
    typedef _Scalar Scalar;
    enum {
        InputsAtCompileTime = nx,
        ValuesAtCompileTime = ny
    };
    typedef Eigen::Matrix<double, InputsAtCompileTime, 1> InputType;
    typedef Eigen::Matrix<double, ValuesAtCompileTime, 1> ValueType;
    typedef Eigen::Matrix<double, ValuesAtCompileTime, InputsAtCompileTime> JacobianType;

    Eigen::Index unknowns, datasetLen;

    NonLinearOptimizationFunctor() : unknowns(InputsAtCompileTime), datasetLen(ValuesAtCompileTime) {}
    NonLinearOptimizationFunctor(i32 unknowns, i32 datasetLen) : unknowns(unknowns), datasetLen(datasetLen) {}

    ptrdiff_t inputs() const {return ptrdiff_t(unknowns);}
    ptrdiff_t values() const {return ptrdiff_t(datasetLen);}
    virtual void operator() (const InputType& , ValueType* , JacobianType*  = 0) const {};
};

struct Basic_functor : NonLinearOptimizationFunctor<double> {
    Basic_functor(Function <i32(const Eigen::VectorXd &b, Eigen::VectorXd &err)> _function) : function(_function) {}
    i32 operator()(const Eigen::VectorXd &b, Eigen::VectorXd &fvec) const {return function(b, fvec);}
    Function <i32(const Eigen::VectorXd &b, Eigen::VectorXd &err)> function;
};

bool NonLinearOptimization(Eigen::VectorXd &y, Eigen::Index numData,
            Function <i32(const Eigen::VectorXd &y, Eigen::VectorXd &residual)>residual,
            double xtol = Null, double ftol = Null, i32 maxfev = Null);
bool SolveNonLinearEquations(Eigen::VectorXd &y, Function <i32(const Eigen::VectorXd &b, Eigen::VectorXd &residual)> Residual,
            double xtol = Null, i32 maxfev = Null, double factor = Null);
double SolveNonLinearEquation(double y, Function <double(double b)> Residual, double xtol = Null, i32 maxfev = Null, double factor = Null);

template <class T>
void Xmlize(XmlIO &xml, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &mat) {
    Size_<int64> sz(mat.cols(), mat.rows());
    xml ("size", sz);
    if(xml.IsStoring()) {
        for(i32 r = 0; r < mat.rows(); r++)
            for(i32 c = 0; c < mat.cols(); c++) {
                XmlIO io = xml.Add("item");
                T data = mat(r, c);
                Xmlize(io, data);
            }
    } else {
        mat.resize(ptrdiff_t(sz.cy), ptrdiff_t(sz.cx));
        i32 r = 0, c = 0;
        for(i32 i = 0; i < xml->GetCount(); i++)
            if(xml->Node(i).IsTag("item")) {
                XmlIO io = xml.At(i);
                T data;
                Xmlize(io, data);
                mat(r, c) = data;
                ++c;
                if (c == sz.cx) {
                    c = 0;
                    r++;
                }
            }
    }
}

template <class T>
void Xmlize(XmlIO &xml, Eigen::Matrix<T, Eigen::Dynamic, 1> &vec) {
    int64 sz = vec.size();
    xml ("size", sz);
    if(xml.IsStoring()) {
        for(i32 r = 0; r < sz; r++) {
            XmlIO io = xml.Add("item");
            T data = vec(r);
            Xmlize(io, data);
        }
    } else {
        vec.resize(ptrdiff_t(sz));
        i32 r = 0;
        for(i32 i = 0; i < xml->GetCount(); i++)
            if(xml->Node(i).IsTag("item")) {
                XmlIO io = xml.At(i);
                T data;
                Xmlize(io, data);
                vec(r++) = data;
            }
    }
}

template <class T>
void Jsonize(JsonIO &io, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &mat) {
    Array<T> vector;
    Size_<int64> sz(mat.cols(), mat.rows());
    io("size", sz);
    if(io.IsStoring()) {
        vector.SetCount(i32(sz.cx)*i32(sz.cy));
        i32 i = 0;
        for(i32 r = 0; r < mat.rows(); r++)
            for(i32 c = 0; c < mat.cols(); c++)
                vector[i++] = mat(r, c);
        io("data", vector);
    } else {
        io("data", vector);
        mat.resize(ptrdiff_t(sz.cy), ptrdiff_t(sz.cx));
        i32 r = 0, c = 0;
        for (i32 i = 0; i < vector.GetCount(); ++i) {
            mat(r, c) = vector[i];
            ++c;
            if (c == sz.cx) {
                c = 0;
                r++;
            }
            if (r == sz.cy)
                break;
        }
    }
}

template <class T>
void Jsonize(JsonIO &io, Eigen::Matrix<T, Eigen::Dynamic, 1> &vec) {
    Array<T> vector;
    int64 sz = vec.size();
    io("size", sz);
    if(io.IsStoring()) {
        vector.SetCount(i32(sz));
        for (i32 i = 0; i < sz; ++i)
            vector[i] = vec(i);
        io("data", vector);
    } else {
        io("data", vector);
        vec.resize(ptrdiff_t(sz));
        for (i32 i = 0; i < vec.size(); ++i)
            vec(i) = vector[i];
    }
}

template <class T>
void Serialize(Stream& stream, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &mat) {
    Size_<int64> sz(mat.cols(), mat.rows());
    stream % sz;
    if(stream.IsStoring()) {
        for(i32 r = 0; r < mat.rows(); r++)
            for(i32 c = 0; c < mat.cols(); c++) {
                T data = mat(r, c);
                stream % data;
            }
    } else {
        mat.resize(ptrdiff_t(sz.cy), ptrdiff_t(sz.cx));
        i32 r = 0, c = 0;
        for(i32 i = 0; i < sz.cy*sz.cx; i++) {
            T data;
            stream % data;
            mat(r, c) = data;
            ++c;
            if (c == sz.cx) {
                c = 0;
                r++;
            }
            if (r == sz.cy)
                break;
        }
    }
}

template <class T>
void Serialize(Stream& stream, Eigen::Matrix<T, Eigen::Dynamic, 1> &vec) {
    int64 sz = vec.size();
    stream % sz;
    if(stream.IsStoring()) {
        for (i32 i = 0; i < sz; ++i) {
            T data = vec(i);
            stream % data;
        }
    } else {
        vec.resize(ptrdiff_t(sz));
        for (i32 i = 0; i < sz; ++i) {
            T data;
            stream % data;
            vec(i) = data;
        }
    }
}

// These functions serve both for Eigen, std and U++ Vectors

template <class Range>
void Resize(Range &v, size_t len) {v.SetCount(i32(len));}
template <class Range>
void Resize(Range &v, size_t len, const typename Range::value_type& init) {
    v.SetCount(i32(len));
    std::fill(v.begin(), v.end(), init);
}

template <class Range>
void ResizeConservative(Range &v, size_t len) {v.SetCount(i32(len));}
template <class Range>
void ResizeConservative(Range &v, size_t len, const typename Range::value_type& init) {v.SetCount(i32(len), init);}
template <class Range>
void Clear(Range &v) {v.Clear();}

template <typename T>
void Resize(Eigen::Matrix<T, Eigen::Dynamic, 1> &v, size_t len) {v.resize(len);}
template <typename T>
void Resize(Eigen::Matrix<T, Eigen::Dynamic, 1> &v, size_t len, const T& init) {v.setConstant(len, 1, init);}
template <typename T>
void ResizeConservative(Eigen::Matrix<T, Eigen::Dynamic, 1> &v, size_t len) {v.conservativeResize(len);}
template <typename T>
void ResizeConservative(Eigen::Matrix<T, Eigen::Dynamic, 1> &v, size_t len, const T& init) {
    size_t len0 = v.size();
    v.conservativeResize(len);
    if (len > len0)
        std::fill(&v[len0], v.data() + len, init);
}
template <typename T>
void Clear(Eigen::Matrix<T, Eigen::Dynamic, 1> &v) {v.resize(0);}
template <typename T>
void Clear(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &m) {m.resize(0, 0);}

template <typename T>
void PrePad(Eigen::Matrix<T, Eigen::Dynamic, 1> &v, size_t len, const T& init) {
    size_t len0 = v.size();
    v.conservativeResize(len);
    if (len > len0) {
        size_t delta = len - len0;
        std::copy(&v[len0 - delta], v.data() + len0, &v[len0]);
        std::copy(v.data(), v.data() + len0 - delta, &v[delta]);
        std::fill(v.data(), v.data() + delta, init);
    }
}


template <typename T>
void Resize(std::vector<T> &v, size_t len) {v.resize(len);}
template <typename T>
void Resize(std::vector<T> &v, size_t len, const T& init) {
    v.resize(len);
    std::fill(v.begin(), v.end(), init);
}
template <typename T>
void ResizeConservative(std::vector<T> &v, size_t len) {v.resize(len);}
template <typename T>
void ResizeConservative(std::vector<T> &v, size_t len, const T& init) {v.resize(len, init);}
template <typename T>
void Clear(std::vector<T> &v) {v.clear();}

#define PostPad ResizeConservative


template <class Range>
auto Begin(const Range &v)              {return v.Begin();}
template <class Range>
auto Begin(Range &v)                    {return v.Begin();}
template <class Range>
auto End(const Range &v)                {return v.End();}
template <class Range>
auto End(Range &v)                      {return v.End();}

template <typename T>
auto Begin(const std::vector<T> &v)     {return v.begin();}
template <typename T>
auto Begin(std::vector<T> &v)           {return v.begin();}
template <typename T>
auto End(const std::vector<T> &v)       {return v.end();}

template <typename T>
auto Begin(const Eigen::Matrix<T, Eigen::Dynamic, 1> &v){return v.data();}
template <typename T>
auto Begin(Eigen::Matrix<T, Eigen::Dynamic, 1> &v)      {return v.data();}
template <typename T>
auto End(const Eigen::Matrix<T, Eigen::Dynamic, 1> &v)  {return v.data() + v.size();}

template <class Range>
auto &First(Range &data) {return data[0];}

template <class Range>
auto &Last(Range &data) {return data[data.size()-1];}

template <class Range>
void Reverse(Range &v) {
    typename Range::value_type *first = v.begin();
    typename Range::value_type *last = v.end();
    while ((first != last) && (first != --last))
        Swap(*first++, *last);
}

template <typename T>
void Reverse(std::vector<T> &v) {std::reverse(v.begin(), v.end());}

template <typename T>
void Reverse(Eigen::Matrix<T, Eigen::Dynamic, 1> &v) {v.reverseInPlace();}


template <class Range1, class Range2>
void Copy(const Range1& in, Range2 &out) {
    Resize(out, in.size());
    std::copy(Begin(in), End(in), Begin(out));
}

template <class T>
void Swap(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &A, i32 rc1, i32 rc2) {
    A.row(rc1).swap(A.row(rc2));    // Swap rows rc1 and rc2
    A.col(rc1).swap(A.col(rc2));    // Swap columns rc1 and rc2
}

template <class T>
void Swap(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &A1, Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &A2, i32 rc1, i32 rc2) {
    ASSERT(A1.rows() == A2.rows() && A1.cols() == A2.cols());

    A1.row(rc1).swap(A2.row(rc2));
    A1.col(rc1).swap(A2.col(rc2));
}

template <typename T>
inline Eigen::Matrix<T, Eigen::Dynamic, 1> Segment(const Eigen::Matrix<T, Eigen::Dynamic, 1> &d, i32 ifrom, i32 num) {
    return d.segment(ifrom, num);
}

template <typename T>
inline std::vector<T> Segment(const std::vector<T> &d, i32 ifrom, i32 num) {
    return std::vector<T>(d.begin() + ifrom, d.begin() + ifrom + num);
}

template <class Range>
inline Range Segment(const Range &d, i32 ifrom, i32 num) {
    Range a;
    if (ifrom + num >= d.size()) {
        num = d.size() - ifrom;
        if (num <= 0)
            return a;
    }
    Resize(a, num);
    std::copy(Begin(d) + ifrom, Begin(d) + ifrom + num, Begin(a));
    return a;
}

template <class Range>
inline Range Mid(const Range &d, i32 ifrom, i32 num) {return Segment(d, ifrom, num);}

template <class Range>
inline Range Left(const Range &d, i32 num) {
    Range a;
    if (num >= d.size()) {
        a = clone(d);
        return a;
    }
    Resize(a, num);
    std::copy(Begin(d), Begin(d) + num, Begin(a));
    return a;
}

template <class Range>
inline Range Right(const Range &d, i32 num) {
    Range a;
    if (num >= d.size()) {
        a = clone(d);
        return a;
    }
    Resize(a, num);
    std::copy(Begin(d) + d.size() - num, Begin(d) + d.size(), Begin(a));
    return a;
}

template <typename T>
inline void Remove(Vec<T> &d, i32 id) {d.Remove(id);}

template <typename T>
inline void Remove(Eigen::Matrix<T, Eigen::Dynamic, 1> &d, i32 id) {
    Eigen::Index sz = d.size();
    Eigen::Index right = sz-id-1;
    d.segment(id, right) = d.segment(id+1, right);
    d.conservativeResize(sz-1);
}

template <class Range>
inline void Remove(Range &d, i32 id) {
    d.erase(d.begin() + id);
}

template <class T>
bool IsNull(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> &a) {return a.size() == 0;}

#define EigenNull   Eigen::MatrixXd()

}


namespace Eigen {

template<> struct NumTraits<drx::Complex> : GenericNumTraits<drx::Complex>
{
  typedef double Real;
  typedef typename NumTraits<double>::Literal Literal;
  enum {
    IsComplex = 1,
    RequireInitialization = NumTraits<Real>::RequireInitialization,
    ReadCost = 2 * NumTraits<Real>::ReadCost,
    AddCost = 2 * NumTraits<Real>::AddCost,
    MulCost = 4 * NumTraits<Real>::MulCost + 2 * NumTraits<Real>::AddCost
  };

  EIGEN_DEVICE_FUNC EIGEN_CONSTEXPR
  static inline Real epsilon() { return NumTraits<Real>::epsilon(); }
  EIGEN_DEVICE_FUNC EIGEN_CONSTEXPR
  static inline Real dummy_precision() { return NumTraits<Real>::dummy_precision(); }
  EIGEN_DEVICE_FUNC EIGEN_CONSTEXPR
  static inline i32 digits10() { return NumTraits<Real>::digits10(); }
};

}
#endif