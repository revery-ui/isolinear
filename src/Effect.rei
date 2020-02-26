type dispatcher('msg) = 'msg => unit;

type t('msg);

let create: (~name: string, unit => unit) => t('msg);
let createWithDispatch: (~name: string, dispatcher('msg) => unit) => t('msg);

let none: t('msg);
let batch: list(t('msg)) => t('msg);
let map: ('a => 'b, t('a)) => t('b);

let name: t('msg) => string;
let run: (t('msg), dispatcher('msg)) => unit;
