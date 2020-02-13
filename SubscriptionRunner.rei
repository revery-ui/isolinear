module type Runner = {
  type t;
  type msg;

  //let empty: unit => t('msg);

  let run: (~dispatch: msg => unit, ~sub: t, t) => t;
};

module Make: (RunnerConfig: {type msg;}) => Runner;
