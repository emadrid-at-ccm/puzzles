constexpr long solve(
    long solution, long available, long availableMask, long position,
    long columns, long diagonals1, long diagonals2
) {
    available &= ~columns;
        // ones indicate column position candidate
    auto solShifted = solution << 4;
    auto pminusone = position - 1;
    while(available) {
        auto hole = 63 - __builtin_clzll(available);
        auto holeBit = 1l << hole;
        available &= ~holeBit;
        auto podiag1 = 32 - hole + position;
        auto diag1bit = 1l << podiag1;
        if(diag1bit & diagonals1) { continue; }
        auto podiag2 = position + hole;
        auto diag2bit = 1l << podiag2;
        if(diag2bit & diagonals2) { continue; }

        // survived all the checks
        if(0 == position) {
            return solShifted | hole;
        }
        // recurr
        auto tentative = solve(
            solShifted | hole,
            availableMask,
            availableMask,
            pminusone,
            columns | holeBit,
            diagonals1 | diag1bit,
            diagonals2 | diag2bit
        );
        if(tentative) { return tentative; }
        available &= ~holeBit;
    }
    return 0;
}

constexpr long nextSolution(long previous, int n) {
    // find the parameters to call 'solve'
    auto backtracked = 0;
    constexpr auto one = 1l;
    long columns = 0l, diagonals1 = 0l, diagonals2 = 0l;
    auto freePosition = 0;
    auto available = (one << n) - 1;
    for(auto row = n; freePosition < --row; ) {
        auto column = (previous >> 4*row) & 0xF;
        columns |= one << column;
        auto diag1 = 32 - column + row;
        auto diag1Bit = one << diag1;
        diagonals1 |= diag1Bit;
        auto diag2 = column + row;
        auto diag2Bit = one << diag2;
        diagonals2 |= diag2Bit;
    }
    do {
        // clear effects at 'freePosition'
        auto row = freePosition;
        auto base = previous >> 4*freePosition;
        auto column = base & 0xF;
        base >>= 4;
        auto columnBit = one << column;
        columns &= ~columnBit;
        auto diag1 = 32 - column + row;
        auto diag1Bit = one << diag1;
        diagonals1 &= ~diag1Bit;
        auto diag2 = column + row;
        auto diag2Bit = one << diag2;
        diagonals2 &= ~diag2Bit;

        // old effects cleared, try the next level
        if(!column) {
            continue;
        }
        auto tentative = solve(base, (1 << column) - 1, available, freePosition, columns, diagonals1, diagonals2);
        if(tentative) { return tentative; }        
    } while(++freePosition < n);
    return 0;
}

constexpr long first(int n) {
    auto mask = (1l << n) - 1;
    return solve(0, mask, mask, n - 1, 0, 0, 0);
}

static_assert(0x1302 == solve(0, 0x3, 0xF, 3, 0, 0, 0), "");

#include <array>

template<long... Pack> struct Longs {
    constexpr static std::array<long, sizeof...(Pack)> value = { Pack... };
};

template<int, long...> struct CS;

template<int N, long Last, long... Previous> struct CS<N, Last, Previous...> {
    constexpr static auto value = nextSolution(Last, N);
    using type = typename CS<N, value, Last, Previous...>::type;
};

template<int N, long... Solutions> struct CS<N, 0l, Solutions...> {
    using type = Longs<Solutions...>;
};

template<int N> struct CS<N> {
    constexpr static auto value = first(N);
    using type = typename CS<N, value>::type;
};

constexpr long convertToDecimal(long v) {
    long rv = 0;
    while(v) {
        rv *= 10;
        auto tmp = v & 0xF;
        rv += tmp;
        v >>= 4;
    }
    return rv;
}

using T = typename CS<8>::type;

auto HEXA = T::value;

template<long... Pack>
constexpr std::array<long, sizeof...(Pack)> asDecimals(Longs<Pack...> *) {
    return { convertToDecimal(Pack)... };
}

auto DECIMALS = asDecimals((T *)nullptr);

