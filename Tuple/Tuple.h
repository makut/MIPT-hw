# include <utility>
# include <cstddef>
# include <type_traits>

template<typename... TArgs>
class Tuple;

template<size_t Ind, typename TFirst, typename... TEnd>
struct getByInd
{
    typedef typename getByInd<Ind - 1, TEnd...>::type type;
};

template<typename TFirst, typename... TEnd>
struct getByInd<0, TFirst, TEnd...>
{
    typedef TFirst type;
};

template<size_t Ind, typename... Types>
using IndType = typename getByInd<Ind, Types...>::type; 

template<size_t Ind, typename... Types>
using NonZeroIndType = typename std::enable_if<Ind != 0, IndType<Ind, Types...> >::type;

template<size_t Ind, typename... Types>
using ZeroIndType = typename std::enable_if<Ind == 0, IndType<Ind, Types...> >::type;

template<typename T1, typename T2>
using CheckSame = typename std::enable_if<std::is_same<T1, T2>::value, T1>::type;

template<typename T1, typename T2>
using CheckDiff = typename std::enable_if<!std::is_same<T1, T2>::value, T1>::type;

template<typename T1, typename T2>
struct Join {};

template<typename... TFirst, typename... TSecond>
struct Join<Tuple<TFirst...>, Tuple<TSecond...> >
{
    using type = Tuple<TFirst..., TSecond...>;
};

template<typename T>
using DelRef = typename std::remove_reference<T>::type;

template<typename Tuple1, typename Tuple2>
using JoinedTuple = typename Join<DelRef<Tuple1>, DelRef<Tuple2> >::type;


template<typename TFirst, typename... TEnd>
class Tuple<TFirst, TEnd...>
{
public:
    Tuple() : val_(), end_() {}

    Tuple(const Tuple&) = default;
    Tuple(Tuple&&) = default;
    Tuple& operator=(const Tuple&) = default;
    Tuple& operator=(Tuple&&) = default;

    Tuple(const TFirst &first, const TEnd&... end) : val_(first), end_(end...) {}

    template<typename TOtherFirst, typename... TOtherEnd, typename = typename std::enable_if<
             !std::is_same<typename std::decay<TOtherFirst>::type, Tuple>::value ||
             sizeof...(TOtherEnd) >::type >
    explicit Tuple(TOtherFirst&& first, TOtherEnd&&... end):
        val_(std::forward<TOtherFirst>(first)), end_(std::forward<TOtherEnd>(end)...) {}

    ~Tuple() = default;

    void swap(Tuple &other)
    {
        Tuple tmp = std::move(other);
        other = std::move(*this);
        *this = std::move(tmp);
    }

    template<size_t Ind, typename... Types>
    friend constexpr ZeroIndType<Ind, Types...>& get(Tuple<Types...>&);

    template<size_t Ind, typename... Types>
    friend constexpr NonZeroIndType<Ind, Types...>& get(Tuple<Types...>&);    

    template<size_t Ind, typename... Types>
    friend constexpr const ZeroIndType<Ind, Types...>& get(const Tuple<Types...>&);

    template<size_t Ind, typename... Types>
    friend constexpr const NonZeroIndType<Ind, Types...>& get(const Tuple<Types...>&);

    template<size_t Ind, typename... Types>
    friend constexpr ZeroIndType<Ind, Types...>&& get(Tuple<Types...>&&);

    template<size_t Ind, typename... Types>
    friend constexpr NonZeroIndType<Ind, Types...>&& get(Tuple<Types...>&&);

    template<typename T, typename TStart, typename... TFinish>
    friend constexpr CheckSame<T, TStart>& get(Tuple<TStart, TFinish...>&);

    template<typename T, typename TStart, typename... TFinish>
    friend constexpr CheckDiff<T, TStart>& get(Tuple<TStart, TFinish...>&);

    template<typename T, typename TStart, typename... TFinish>
    friend constexpr const CheckSame<T, TStart>& get(const Tuple<TStart, TFinish...>&);

    template<typename T, typename TStart, typename... TFinish>
    friend constexpr const CheckDiff<T, TStart>& get(const Tuple<TStart, TFinish...>&);

    template<typename T, typename TStart, typename... TFinish>
    friend constexpr CheckSame<T, TStart>&& get(Tuple<TStart, TFinish...>&&);

    template<typename T, typename TStart, typename... TFinish>
    friend constexpr CheckDiff<T, TStart>&& get(Tuple<TStart, TFinish...>&&);

    template<typename T1, typename... Tail1, typename T2, typename... Tail2>
    friend constexpr bool operator==(const Tuple<T1, Tail1...>&, const Tuple<T2, Tail2...>&);

    template<typename T1, typename... Tail1, typename T2, typename... Tail2>
    friend constexpr bool operator<(const Tuple<T1, Tail1...>&, const Tuple<T2, Tail2...>&);

    template<typename Tuple1, typename Tuple2>
    constexpr typename std::enable_if<!DelRef<Tuple1>::empty_,
                                      JoinedTuple<Tuple1, Tuple2> >::type
    friend concatenateTwo(Tuple1&&, Tuple2&&);
private:
    TFirst val_;
    Tuple<TEnd...> end_;
    constexpr static const bool empty_ = false;
};

template<>
class Tuple<>
{
public:
    Tuple() = default;
    Tuple(const Tuple&) = default;
    Tuple(Tuple&&) = default;
    Tuple& operator=(const Tuple&) = default;
    Tuple& operator=(Tuple&&) = default;
    ~Tuple() = default;

    void swap(Tuple&) {}

    template<typename Tuple2>
    constexpr DelRef<Tuple2> concatenateTwo(const Tuple<>&, Tuple2&& tuple2);
private:
    constexpr static const bool empty_ = true;
};

template<class T>
struct UnwrapRefwrapper
{
    using type = T;
};
 
template<class T>
struct UnwrapRefwrapper<std::reference_wrapper<T> >
{
    using type = T&;
};
 
template<class T>
using SpecialDecay = typename UnwrapRefwrapper<typename std::decay<T>::type>::type;
 
template <class... Types>
auto makeTuple(Types&&... args)
{
    return Tuple<SpecialDecay<Types>...>(std::forward<Types>(args)...);
}

template<size_t Ind, typename... Types>
constexpr NonZeroIndType<Ind, Types...>& get(Tuple<Types...> &tuple)
{
    return get<Ind - 1>(tuple.end_);
}

template<size_t Ind, typename... Types>
constexpr ZeroIndType<Ind, Types...>& get(Tuple<Types...> &tuple)
{
    return tuple.val_;
}

template<size_t Ind, typename... Types>
constexpr const NonZeroIndType<Ind, Types...>& get(const Tuple<Types...> &tuple)
{
    return get<Ind - 1>(tuple.end_);
}

template<size_t Ind, typename... Types>
constexpr const ZeroIndType<Ind, Types...>& get(const Tuple<Types...> &tuple)
{
    return tuple.val_;
}

template<size_t Ind, typename... Types>
constexpr NonZeroIndType<Ind, Types...>&& get(Tuple<Types...>&& tuple)
{
    return std::move(get<Ind - 1>(std::move(tuple.end_)));
}

template<size_t Ind, typename... Types>
constexpr ZeroIndType<Ind, Types...>&& get(Tuple<Types...>&& tuple)
{
    return std::move(tuple.val_);
}

template<typename T, typename TFirst, typename... TEnd>
constexpr CheckDiff<T, TFirst>& get(Tuple<TFirst, TEnd...> &tuple)
{
    return get<T>(tuple.end_);
}

template<typename T, typename TFirst, typename... TEnd>
constexpr CheckSame<T, TFirst>& get(Tuple<TFirst, TEnd...> &tuple)
{
    return tuple.val_;
}

template<typename T, typename TFirst, typename... TEnd>
constexpr const CheckDiff<T, TFirst>& get(const Tuple<TFirst, TEnd...> &tuple)
{
    return get<T>(tuple.end_);
}

template<typename T, typename TFirst, typename... TEnd>
constexpr const CheckSame<T, TFirst>& get(const Tuple<TFirst, TEnd...> &tuple)
{
    return tuple.val_;
}

template<typename T, typename TFirst, typename... TEnd>
constexpr CheckDiff<T, TFirst>&& get(Tuple<TFirst, TEnd...> &&tuple)
{
    return std::move(get<T>(std::move(tuple.end_)));
}

template<typename T, typename TFirst, typename... TEnd>
constexpr CheckSame<T, TFirst>&& get(Tuple<TFirst, TEnd...> &&tuple)
{
    return std::move(tuple.val_);
}

constexpr bool operator==(const Tuple<>&, const Tuple<>&)
{
    return true;
}

template<typename T1, typename... Tail1, typename T2, typename... Tail2>
constexpr bool operator==(const Tuple<T1, Tail1...> &first, const Tuple<T2, Tail2...> &second)
{
    return (first.val_ == second.val_ && first.end_ == second.end_);
}

constexpr bool operator<(const Tuple<>&, const Tuple<>&)
{
    return false;
}

template<typename T1, typename... Tail1, typename T2, typename... Tail2>
constexpr bool operator<(const Tuple<T1, Tail1...> &first, const Tuple<T2, Tail2...> &second)
{
    static_assert(sizeof...(Tail1) == sizeof...(Tail2), "Different sizes tuples are incomparable");
    return (first.val_ < second.val_ || (first.val_ == second.val_ && first.end_ < second.end_));
}

template<typename... Types1, typename... Types2>
constexpr bool operator!=(const Tuple<Types1...> &first, const Tuple<Types2...> &second)
{
    return !(first == second);
}

template<typename... Types1, typename... Types2>
constexpr bool operator>(const Tuple<Types1...> &first, const Tuple<Types2...> &second)
{
    return second < first;
}

template<typename... Types1, typename... Types2>
constexpr bool operator<=(const Tuple<Types1...> &first, const Tuple<Types2...> &second)
{
    return (first < second || first == second);
}

template<typename... Types1, typename... Types2>
constexpr bool operator>=(const Tuple<Types1...> &first, const Tuple<Types2...> &second)
{
    return (first > second || first == second);
}

template<typename Tuple1, typename Tuple2>
constexpr typename std::enable_if<!DelRef<Tuple1>::empty_, JoinedTuple<Tuple1, Tuple2> >::type
          concatenateTwo(Tuple1&& tuple1, Tuple2&& tuple2)
{
    return JoinedTuple<Tuple1, Tuple2>(std::forward<decltype(tuple1.val_)>(tuple1.val_),
           concatenateTwo(std::forward<decltype(tuple1.end_)>(tuple1.end_), std::forward<Tuple2>(tuple2)));
}

template<typename Tuple2>
constexpr DelRef<Tuple2> concatenateTwo(const Tuple<>&, Tuple2&& tuple2)
{
    return std::forward<Tuple2>(tuple2);
}

template<typename Tuple1>
DelRef<Tuple1> tupleCat(Tuple1&& tuple1)
{
    return std::forward<Tuple1>(tuple1);
}

template<typename Tuple, typename... Tail>
auto tupleCat(Tuple&& tuple, Tail... tail)
{
    return concatenateTwo(std::forward<Tuple>(tuple), tupleCat(tail...));
}