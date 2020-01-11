type dispatchFunction('a) = 'a => unit;

type t('a);

let create: (~name: string, unit => unit) => t('a);

let createWithDispatch:
  (~name: string, dispatchFunction('a) => unit) => t('a);

let getName: t('a) => string;

let none: t('a);

let run: (t('a), dispatchFunction('a)) => unit;

let batch: list(t('a)) => t('a);

let map: ('a => 'b, t('a)) => t('b);
