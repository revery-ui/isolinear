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

  module Deprecated {
    let getStoreStream: unit => Stream.t((model, msg));
  }
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
