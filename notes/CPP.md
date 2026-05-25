# Templates

## Two-phase name lookup

When the compiler parses a template, name lookup happens in two phases:

1. **At template definition time** — non-dependent names are resolved immediately, in the context where the template is defined. If a name doesn't depend on a template parameter, the compiler must be able to find it now.
2. **At instantiation time** — dependent names (anything whose meaning depends on a template parameter) are resolved later, once the template arguments are known, in the context of the instantiation.

The consequence that bites people: unqualified names in a template are **not** looked up in dependent base classes during phase 1. Given `template <typename T> struct Derived : Base<T>`, a bare `foo()` inside `Derived` won't find `Base<T>::foo` because `Base<T>` is dependent and the compiler refuses to peek inside it until `T` is known. The fix is to make the call dependent: `this->foo()`, `Base<T>::foo()`, or a `using Base<T>::foo;` declaration.

The motivating reason: a specialization of `Base<T>` for some specific `T` could remove `foo` entirely (or replace the whole interface), so the compiler cannot bind `foo` early without risking a silent mismatch later.

## Why specializations can diverge arbitrarily from the primary template

A full specialization is an independent class definition that happens to share a name with the primary template. It does not inherit, override, or extend — it replaces. This is intentional, because specialization exists precisely for cases where the generic implementation cannot or should not apply to a specific type.

### `std::vector<bool>`

The primary `std::vector<T>` stores a contiguous array of `T` and gives you `T&` from `operator[]`. For `T = bool`, the standard library specializes `std::vector<bool>` to pack elements as individual bits — typically eight booleans per byte — to save space.

This breaks the primary's interface in observable ways:

- `operator[]` cannot return `bool&` because a single bit isn't addressable. It returns a **proxy reference** (`std::vector<bool>::reference`) that overloads assignment to flip the underlying bit.
- `&v[0]` does not yield a `bool*`. Code that assumes `vector<T>` always exposes a contiguous `T*` (e.g., passing it to a C API) silently breaks for `T = bool`.
- `std::vector<bool>` is not a `Container` in the standard sense — generic algorithms templated on `vector<T>::reference == T&` misbehave.

It's the canonical example of why specializations are unconstrained: the bit-packed layout is genuinely incompatible with the primary's contract, and forcing it to preserve `T&` semantics would defeat the optimization. It's also the canonical example of why this freedom is dangerous — `vector<bool>` is widely regarded as a design mistake precisely because it diverges from the primary in ways generic code can't detect.

### `std::is_integral`

Type traits use specialization as a metaprogramming mechanism. The "interface" is just a static `::value`, and divergence between specializations is the entire point.

The primary template defaults to false:

```cpp
template <typename T>
struct is_integral : std::false_type {};
```

Then each integral type gets its own specialization flipping the answer to true:

```cpp
template <> struct is_integral<bool>           : std::true_type {};
template <> struct is_integral<char>           : std::true_type {};
template <> struct is_integral<signed char>    : std::true_type {};
template <> struct is_integral<unsigned char>  : std::true_type {};
template <> struct is_integral<short>          : std::true_type {};
template <> struct is_integral<int>            : std::true_type {};
// ... etc for every integral type, plus cv-qualified variants
```

There is no "generic" implementation that could compute this — the answer is per-type by definition. Specialization here is not an override of fallible default behavior; it is the data. Every trait in `<type_traits>` (`is_pointer`, `is_floating_point`, `is_same`, ...) follows the same pattern, and user-extensible traits like `std::hash` and `std::formatter` rely on the same mechanism for users to plug in their own types.
