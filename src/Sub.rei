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

type t('msg) = Sub_Internal.t('msg);

let batch: list(t('msg)) => t('msg);

module type Sub = {
  type params;
  type msg;

  let create: params => t(msg);
};

module Make: (ConfigInfo: Config) => Sub with type msg = ConfigInfo.msg and type params = ConfigInfo.params;

let none: t('msg);
