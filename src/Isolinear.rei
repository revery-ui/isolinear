type dispatcher('msg) = 'msg => unit;
type unsubscribe = unit => unit;

module Stream: {
  type t('msg);

  let ofDispatch: (dispatcher('msg) => unit) => t('msg);
  let create: unit => (t('msg), dispatcher('msg));
  let subscribe: (t('msg), dispatcher('msg)) => unsubscribe;
  let map: (t('a), 'a => option('b)) => t('b);
  let connect: (dispatcher('msg), t('msg)) => unsubscribe;
};

module Effect: {
  type t('msg);

  let create: (~name: string, unit => unit) => t('a);
  let createWithDispatch:
    (~name: string, dispatcher('msg) => unit) => t('msg);
  let getName: t(_) => string;
  let none: t(_);
  let run: (t('msg), dispatcher('msg)) => unit;
  let batch: list(t('msg)) => t('msg);
  let map: ('a => 'b, t('a)) => t('b);
};

module Updater: {
  type t('model, 'msg) = ('model, 'msg) => ('model, Effect.t('msg));

  let ofReducer: (('model, 'msg) => 'model) => t('model, 'msg);
  let combine: list(t('model, 'msg)) => t('model, 'msg);
};

module Sub: {
  type t('msg);

  let batch: list(t('msg)) => t('msg);
  let map: ('a => 'b, t('a)) => t('b);
  let none: t('msg);

  module type Sub = {
    type params;
    type msg;

    let create: params => t(msg);
  };

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

  module Make:
    (ConfigInfo: Config) =>
     Sub with type msg = ConfigInfo.msg and type params = ConfigInfo.params;
};

module Store: {
  module type Store = {
    type msg;
    type model;

    let updater: Updater.t(model, msg);
    let subscriptions: model => Sub.t(msg);

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
        let updater: Updater.t(model, msg);
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
