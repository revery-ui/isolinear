module Stream = Stream;

module Effect: {
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
};

module Updater: {
  type t('msg, 'model) = ('model, 'msg) => ('model, Effect.t('msg));

  let ofReducer: (('model, 'msg) => 'model) => t('msg, 'model);
  let combine: list(t('msg, 'model)) => t('msg, 'model);
};

module Sub: {
  module type Config = {
    type params;
    type msg;

    // State that is carried by the subscription while it is active
    type state;

    let subscriptionName: string;

    let getUniqueId: params => string;

    let init: (~params: params, ~dispatch: msg => unit) => state;
    let update:
      (~params: params, ~state: state, ~dispatch: msg => unit) => state;
    let dispose: (~params: params, ~state: state) => unit;
  };

  type t('msg);

  let batch: list(t('msg)) => t('msg);

  let map: ('a => 'b, t('a)) => t('b);

  module type Sub = {
    type params;
    type msg;

    let create: params => t(msg);
  };

  module Make:
    (ConfigInfo: Config) =>
     Sub with type msg = ConfigInfo.msg and type params = ConfigInfo.params;

  let none: t('msg);
};

module Store: {
  module type Store = {
    type msg;
    type model;

    let updater: Updater.t(msg, model);
    let subscriptions: model => Sub.t(msg);

    type unsubscribe = unit => unit;

    let getModel: unit => model;
    let dispatch: msg => unit;

    let onBeforeMsg: (msg => unit) => unsubscribe;
    let onModelChanged: (model => unit) => unsubscribe;
    let onAfterMsg: ((msg, model) => unit) => unsubscribe;

    let onBeforeEffectRan: (Effect.t(msg) => unit) => unsubscribe;
    let onPendingEffect: (unit => unit) => unsubscribe;
    let onAfterEffectRan: (Effect.t(msg) => unit) => unsubscribe;

    let hasPendingEffects: unit => bool;
    let runPendingEffects: unit => unit;

    module Deprecated: {let getStoreStream: unit => Stream.t((model, msg));};
  };

  module Make:
    (
      Config: {
        type msg;
        type model;

        let initial: model;
        let updater: Updater.t(msg, model);
        let subscriptions: model => Sub.t(msg);
      },
    ) =>
     Store with type msg = Config.msg and type model = Config.model;
};

module Internal: {
  module SubscriptionRunner: {
    module Make:
      (RunnerConfig: {type msg;}) =>
       {
        type t;
        type msg = RunnerConfig.msg;

        let empty: t;
        let run: (~dispatch: msg => unit, ~sub: Sub.t(msg), t) => t;
      };
  };
};
