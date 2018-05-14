#pragma once
#include <utility>

using std::forward;
using std::move;
using std::enable_if;


template <size_t N, typename FIRST, typename... OTHERS>
struct KthType {
    typedef typename KthType<N-1, OTHERS...>::type type;
};

template <typename FIRST, typename... OTHERS>
struct KthType<1, FIRST, OTHERS...> {
    typedef FIRST type;
};


template <typename FIRST, typename... OTHERS>
class Tuple {
public:
    Tuple() = default;

    template <typename... ARGS>
    explicit Tuple(const FIRST&, ARGS&&...);
    template <typename... ARGS>
    explicit Tuple(FIRST&&, ARGS&&...);

    Tuple(const Tuple<FIRST, OTHERS...>&) = default;
    Tuple(Tuple<FIRST, OTHERS...>&&);

    Tuple<FIRST, OTHERS...>& operator=(const Tuple<FIRST, OTHERS...>&);
    Tuple<FIRST, OTHERS...>& operator=(Tuple<FIRST, OTHERS...>&&);

    ~Tuple() = default;

    typedef Tuple<OTHERS...> Son;

    //friends

    template <size_t N, typename FST, typename... OTH>
    friend class GetByNum;

    template <typename T, typename FST, typename... OTH>
    friend class GetByType;

    template <typename FST, typename... OTH>
    friend class Compares;

private:
    FIRST _fst;
    Tuple<OTHERS...> _others;
};

template <typename T>
class Tuple<T> {
public:
    Tuple() = default;
    explicit Tuple(const T&);
    explicit Tuple(T&&);

    Tuple(const Tuple<T>&) = default;
    Tuple(Tuple<T>&&);

    Tuple<T>& operator=(const Tuple<T>&);
    Tuple<T>& operator=(Tuple<T>&&);

    ~Tuple() = default;

    //friends

    template <size_t N, typename FST, typename... OTH>
    friend class GetByNum;

    template <typename U, typename FST, typename... OTH>
    friend class GetByType;

    template <typename FST, typename... OTH>
    friend class Compares;friend bool operator==(const Tuple&, const Tuple&);

private:
    T _fst;

};

//        Implementation

// Construct

template <typename FIRST, typename... OTHERS>
template <typename... ARGS>
Tuple<FIRST, OTHERS...>::Tuple(FIRST&& farg, ARGS&&... args):
        _fst(move(farg)),
        _others(forward<ARGS>(args)...) {}

template <typename FIRST, typename... OTHERS>
template <typename... ARGS>
Tuple<FIRST, OTHERS...>::Tuple(const FIRST& farg, ARGS&&... args):
        _fst(farg),
        _others(forward<ARGS>(args)...) {}

template <typename T>
Tuple<T>::Tuple(const T& value):_fst(value) {}

template <typename T>
Tuple<T>::Tuple(T&& value):_fst(move(value)) {}


template <typename FIRST, typename... OTHERS>
Tuple<FIRST, OTHERS...>::Tuple(Tuple<FIRST, OTHERS...>&& tuple):
        _fst(move(tuple._fst)),
        _others(move(tuple._others)) {}

template <typename T>
Tuple<T>::Tuple(Tuple<T>&& tuple):
        _fst(move(tuple._fst)) {}


template <typename FIRST, typename... ARGS>
Tuple<FIRST, ARGS...>& Tuple<FIRST, ARGS...>::operator=
(Tuple<FIRST, ARGS...>&& tuple) {
    _fst = move(tuple._fst);
    _others = move(tuple._others);
    return *this;
}

template <typename T>
Tuple<T>& Tuple<T>::operator=(Tuple<T>&& tuple) {
    _fst = move(tuple._fst);
    return *this;
}

// Copy

template <typename FIRST, typename... ARGS>
Tuple<FIRST, ARGS...>& Tuple<FIRST, ARGS...>::operator=
        (const Tuple<FIRST, ARGS...>& tuple) {
    _fst = tuple._fst;
    _others = tuple._others;
    return *this;
}

template <typename T>
Tuple<T>& Tuple<T>::operator=(const Tuple<T>& tuple) {
    _fst = tuple._fst;
    return *this;
}


template <typename... ARGS>
Tuple<std::remove_const_t<std::remove_reference_t<ARGS> >...>
makeTuple(ARGS&&... args) {
    return Tuple<std::remove_const_t<std::remove_reference_t<ARGS> >...>
            (forward<ARGS>(args)...);
}

// Get By Num

template <size_t N, typename FIRST, typename... OTHERS>
struct GetByNum {
    static decltype(auto)
    action(Tuple<FIRST, OTHERS...>& tuple) {
        return GetByNum<N-1, OTHERS...>::action(tuple._others);
    }

    static decltype(auto)
    action(const Tuple<FIRST, OTHERS...>& tuple) {
        return GetByNum<N-1, OTHERS...>::action(tuple._others);
    }

    static decltype(auto)
    action(Tuple<FIRST, OTHERS...>&& tuple) {
        return GetByNum<N-1, OTHERS...>::action(move(tuple._others));
    }
};

template <typename FIRST, typename... OTHERS>
struct GetByNum<1, FIRST, OTHERS...> {
    static FIRST& action(Tuple<FIRST, OTHERS...>& tuple) {
        return tuple._fst;
    }

    static const FIRST& action(const Tuple<FIRST, OTHERS...>& tuple) {
        return tuple._fst;
    }

    static FIRST&& action(Tuple<FIRST, OTHERS...>&& tuple) {
        return move(tuple._fst);
    }
};

template <size_t N, typename FIRST, typename... OTHERS>
decltype(auto) get(Tuple<FIRST, OTHERS...>& tuple) {
    return GetByNum<N, FIRST, OTHERS...>::action(tuple);
};

template <size_t N, typename FIRST, typename... OTHERS>
decltype(auto) get(const Tuple<FIRST, OTHERS...>& tuple) {
    return GetByNum<N, FIRST, OTHERS...>::action(tuple);
};

template <size_t N, typename FIRST, typename... OTHERS>
decltype(auto) get(Tuple<FIRST, OTHERS...>&& tuple) {
    return GetByNum<N, FIRST, OTHERS...>::action(move(tuple));
};

// Get By Type

template <typename T, typename FIRST, typename... OTHERS>
struct GetByType {
    static T& action (Tuple<FIRST, OTHERS...>& tuple) {
        return GetByType<T, OTHERS...>::action(tuple._others);
    }

    static const T& action (const Tuple<FIRST, OTHERS...>& tuple) {
        return GetByType<T, OTHERS...>::action(tuple._others);
    }

    static T&& action (Tuple<FIRST, OTHERS...>&& tuple) {
        return GetByType<T, OTHERS...>::action(move(tuple._others));
    }
};

template <typename T, typename... OTHERS>
struct GetByType<T, T, OTHERS...> {
    static T& action (Tuple<T, OTHERS...>& tuple) {
        return tuple._fst;
    }

    static T&& action (Tuple<T, OTHERS...>&& tuple) {
        return move(tuple._fst);
    }

    static const T& action (const Tuple<T, OTHERS...>& tuple) {
        return tuple._fst;
    }
};

template <typename T, typename FIRST, typename... OTHERS>
T& get(Tuple<FIRST, OTHERS...>& tuple) {
    return GetByType<T, FIRST, OTHERS...>::action(tuple);
};

template <typename T, typename FIRST, typename... OTHERS>
const T& get(const Tuple<FIRST, OTHERS...>& tuple) {
    return GetByType<T, FIRST, OTHERS...>::action(tuple);
};

template <typename T, typename FIRST, typename... OTHERS>
T&& get(Tuple<FIRST, OTHERS...>&& tuple) {
    return GetByType<T, FIRST, OTHERS...>::action(move(tuple));
};

// Compare

template <typename FIRST, typename... OTHERS>
struct Compares {
    static bool equial(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
        if (fst._fst == scnd._fst) {
            return Compares<OTHERS...>::equial(fst._others, scnd._others);
        }
        else {
            return false;
        }
    };

    static bool less(const Tuple<FIRST, OTHERS...>& fst,
              const Tuple<FIRST, OTHERS...>& scnd) {
        if (fst._fst == scnd._fst) {
            return Compares<OTHERS...>::less(fst._others, scnd._others);
        }
        else {
            return fst._fst < scnd._fst;
        }
    };
};

template <typename T>
struct Compares<T> {
    static bool equial(const Tuple<T>& fst,
                const Tuple<T>& scnd) {
        return fst._fst == scnd._fst;
    }

    static bool less(const Tuple<T>& fst,
              const Tuple<T>& scnd) {
        return fst._fst < scnd._fst;
    }
};

template <typename FIRST, typename... OTHERS>
bool operator==(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
    return Compares<FIRST, OTHERS...>::equial(fst, scnd);
};

template <typename FIRST, typename... OTHERS>
bool operator!=(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
    return not (fst == scnd);
};

template <typename FIRST, typename... OTHERS>
bool operator<(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
    return Compares<FIRST, OTHERS...>::less(fst, scnd);
};

template <typename FIRST, typename... OTHERS>
bool operator>(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
    return scnd < fst;
};

template <typename FIRST, typename... OTHERS>
bool operator>=(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
    return not (fst < scnd);
};

template <typename FIRST, typename... OTHERS>
bool operator<=(const Tuple<FIRST, OTHERS...>& fst,
                const Tuple<FIRST, OTHERS...>& scnd) {
    return not (scnd < fst);
};

