type dispatchFunction('a) = 'a => unit;

type t('a) = {
  name: string,
  f: dispatchFunction('a) => unit,
};

let create = (~name: string, f: unit => unit) => {name, f: _ => f()};

let createWithDispatch = (~name: string, f: dispatchFunction('a) => unit) => {
  name,
  f,
};

let none: t('a) = {name: "None", f: _ => ()};

let run = (effect: t('a), dispatch: dispatchFunction('a)) => {
  effect.f(dispatch);
};

let batch = (effects: list(t('a))) => {
  let execute = dispatch => {
    List.iter(e => run(e, dispatch), effects);
  };

  {name: "batched effects", f: execute};
};

/* let batch: (~name: string, List(t)) => Effect.t; */
/* let create: (~name: string, ~f:effectFunction, ()) => Effect.t; */
