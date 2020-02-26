// Borrowed from ReactMini's `handedOffInstance` - the "pipe" is a mechanism for
// communicating between instances sharing the same pipe, in the presence of
// existentials where it's impossible to prove that the two values of the same
// type actually are of the same type. The pipe allows the value to "coerced"
// from one type to another in a type-safe manner. If the type is not the same,
// the "input" and "output" pipes also cannot be the same, and hence passing a
// value into the "input" pipe will not see anything come out of the "output".
// 'send' will then simply return `None`.

type t('data);

let create: unit => t('data);
let send: (t('a), t('b), 'a) => option('b);
