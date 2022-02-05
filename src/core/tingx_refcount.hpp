#ifndef SRC_CORE_TINGX_REFCOUNT_HPP_
#define SRC_CORE_TINGX_REFCOUNT_HPP_

namespace tingx {

template <typename T>
class Ptr;

class RefCount {
public:
    virtual ~RefCount() {}

private:
    template <typename T>
    friend class Ptr;

    virtual size_t AddRef() = 0;
    virtual size_t Release() = 0;
};

class RefCounted : virtual public RefCount {
public:
    RefCounted() : ref_count_(0) { }
    RefCounted(const RefCounted&) : ref_count_(0) {}
    virtual ~RefCounted() {}

private:
  virtual size_t AddRef() {
    return ++ref_count_;
  }

  virtual size_t Release() {
    return --ref_count_;
  }

  mutable size_t ref_count_;
};



template <typename T>
class Ptr {
 public:
  Ptr() : p_(nullptr) {}

  Ptr(T* pT) : p_(nullptr) {
    *this = pT;
  }

  Ptr(const Ptr<T>& p) : p_(nullptr) {
    *this = p;
  }

  ~Ptr() {
    Release();
  }

  T* operator=(T* pT) {
    if (p_ == pT) {
      return p_;
    }
    if (pT) {
      RefCount* p = static_cast<RefCount*>(pT);
      if (p == NULL) {
        return NULL;
      }
      p->AddRef();
    }
    Release();
    p_ = pT;
    return p_;
  }

  T* operator=(const Ptr<T>& p) {
    if (p_ == p.p_) {
      return p_;
    }
    return operator=(p.p_);
  }

  operator T*&() {
    return p_;
  }

  T& operator*() const {
    return *p_;
  }

  T* operator->() const {
    return p_;
  }

  bool operator!() const {
    return (p_ == NULL);
  }

  bool operator!=(T* pT) const {
    return !operator==(pT);
  }

  bool operator==(T* pT) const {
    return (p_ == pT);
  }

  size_t Release() const {
    size_t ref_count = 0;
    if (p_) {
      RefCount* p = static_cast<RefCount*>(p_);
      if (p) {
        ref_count = p->Release();
      }
      p_ = NULL;
    }
    return ref_count;
  }

  void Attach(T* pT) {
    if (p_ != pT) {
      Release();
      p_ = pT;
    }
  }

  T* Detach() {
    T* pT = p_;
    p_ = NULL;
    return pT;
  }

  mutable T* p_;
};



}


#endif