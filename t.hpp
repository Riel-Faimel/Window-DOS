template<typename ...Type>
struct tuple;

template<typename Fir, typename... Args>
struct tuple<Fir, Args...> : public tuple<Args...> {
    Fir data;
    tuple(Fir f, Args... args):
    data{f}, tuple<Args...>{args...}{}

    tuple(tuple<Fir, Args...>& t):
    data{t.data}, tuple<Args...>{static_cast<tuple<Args...>&>(t)}{}

    tuple(tuple<Fir, Args...>&& t):
    data{t.data}, tuple<Args...>{static_cast<tuple<Args...>&&>(t)}{
        t.data = Fir{};
    }
};

template<>
struct tuple<> {
public:
    tuple() = default;
};

template<typename... Args>
tuple<Args...> make_tuple(Args... args) {
    return tuple<Args...>{args...};
}

template<unsigned long long N, typename Fir, typename... Args>
auto& get(tuple<Fir, Args...>& t) {
    if constexpr (N==0) return t.data;
    else return get<N-1>(static_cast<tuple<Args...>&>(t));
}

template<unsigned long long N, typename Fir, typename... Args>
const auto& get(const tuple<Fir, Args...>& t) {
    if constexpr (N==0)return t.data;
    else return get<N-1>(static_cast<tuple<Args...>&>(t));
}

template<unsigned long long N, typename Fir, typename... Args>
auto&& get(tuple<Fir, Args...>&& t) {
    if constexpr (N==0)return bits::forward(t.data);
    else return get<N-1>(static_cast<tuple<Args...>&&>(t));
}

template<typename... T>
struct tuple_size;

template<typename Fir, typename... Args>
struct tuple_size<tuple<Fir, Args...>> {
    constexpr static unsigned long long value = tuple_size<Args...>::value + 1;
};

template<>
struct tuple_size<tuple<>> {
    constexpr static unsigned long long value = 0;
};

template<unsigned long long I, typename... T>
struct tuple_type;

template<unsigned long long I, typename Fir, typename... Args>
struct tuple_type<I, tuple<Fir, Args...>> {
    using type = tuple_type<I-1, tuple<Args...>>::type;
};

template<typename Fir, typename... Args>
struct tuple_type<0, tuple<Fir, Args...>> {
    using type = Fir;
};

template<unsigned long long I>
struct tuple_type<I, tuple<>> {
    using type = void;
};