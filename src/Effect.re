type effectFunction;
type t = {
  name: string,
  f: unit => unit,
};

let create = (~name: string, f) => {
    name,
    f
};

let none: t = {name: "None", f: () => ()};

let run = (effect: t) => {
  effect.f();
};

let batch = (effects: list(t)) => {
   let execute = () => {
       List.iter((e) => run(e), effects);
   };

   {
    name: "batched effects",
    f: execute,
   };
};

/* let batch: (~name: string, List(t)) => Effect.t; */
/* let create: (~name: string, ~f:effectFunction, ()) => Effect.t; */
