type t('msg);

let none: t('msg);

module type Config = {
  type params;
  type msg;

  // State that is carried by the subscription while it is active
  type state;

  let subscriptionName: string;

  let getUniqueId: params => string;

  let start: (~params: params, ~dispatch: msg => unit) => state;
  let update:
    (~params: params, ~state: state, ~dispatch: msg => unit) => state;
  let dispose: (~params: params, ~state: state) => unit;
};

module Make:
  (Config) =>
   {
    type msg;
    type params;

    let create: params => t(msg);
  };
