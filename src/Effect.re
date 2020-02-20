type dispatcher('msg) = 'msg => unit;

type effect('msg) = {
  getName: int => string,
  f: dispatcher('msg) => unit,
};

type t('msg) = option(effect('msg));

module Internal = {
  let indentName = (name, level) => String.make(level, ' ') ++ name;
};

let create = (~name: string, f: unit => unit) =>
  Some({getName: Internal.indentName(name), f: _ => f()});

let createWithDispatch = (~name: string, f: dispatcher('msg) => unit) =>
  Some({getName: Internal.indentName(name), f});

let none: t('msg) = None;

let run = (effect: t('msg), dispatch: dispatcher('msg)) =>
  Option.iter(eff => eff.f(dispatch), effect);

let batch = (effects: list(t('msg))) => {
  let effects = effects |> List.filter(eff => eff != None);

  let execute = dispatch => List.iter(e => run(e, dispatch), effects);

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
  | _ => Some({getName, f: execute})
  };
};

let map = f =>
  Option.map(eff =>
    {...eff, f: dispatch => eff.f(msg => dispatch(f(msg)))}
  );

let name =
  fun
  | Some(eff) => eff.getName(0)
  | None => "(None)";
