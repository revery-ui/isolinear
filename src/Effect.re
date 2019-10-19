type dispatchFunction('a) = 'a => unit;

type effect('a) = {
  getName: int => string,
  f: dispatchFunction('a) => unit,
};

type t('a) = option(effect('a));

let _namePrinter = (name, indentLevel) => {
  String.make(indentLevel, ' ') ++ name;
};

let getName = (v: t('a)) =>
  switch (v) {
  | None => "(None)"
  | Some(v) => v.getName(0)
  };

let create = (~name: string, f: unit => unit) =>
  Some({getName: _namePrinter(name), f: _ => f()});

let createWithDispatch = (~name: string, f: dispatchFunction('a) => unit) =>
  Some({getName: _namePrinter(name), f});

let none: t('a) = None;

let run = (effect: t('a), dispatch: dispatchFunction('a)) => {
  switch (effect) {
  | None => ()
  | Some(effect) => effect.f(dispatch)
  };
};

let batch = (effects: list(t('a))) => {
  let effects = effects |> List.filter(e => e != None);

  let execute = dispatch => {
    List.iter(e => run(e, dispatch), effects);
  };

  let getName = indentLevel => {
    let start = String.make(indentLevel, ' ') ++ "Batch" ++ ":";

    start
    ++ List.fold_left(
         (prev, curr) => {
           switch (curr) {
           | None => prev
           | Some(curr) =>
             let newName = curr.getName(indentLevel + 1) ++ "\n";
             prev ++ newName;
           }
         },
         "\n",
         effects,
       );
  };

  switch (effects) {
  | [] => None
  | v => Some({getName, f: execute})
  };
};

/* let batch: (~name: string, List(t)) => Effect.t; */
/* let create: (~name: string, ~f:effectFunction, ()) => Effect.t; */
