type effectFunction;
type t('a) = {
  name: string,
  f: unit => option('a),
};

let none: t('a) = {name: "None", f: () => None};

let run = (effect: t('a)) => {
  let _ = effect.f();
  ();
};
/* let batch: (~name: string, List(t)) => Effect.t; */
/* let create: (~name: string, ~f:effectFunction, ()) => Effect.t; */
