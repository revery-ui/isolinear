module type Store = {
  type msg;
  type model;

  let updater: Updater.t(msg, model);
  let subscriptions: model => Sub.t(msg);

  type unsubscribe = unit => unit;

  let getModel: unit => model;
  let dispatch: msg => unit;

  let onModelChanged: (model => unit) => unsubscribe;
  let onPendingEffect: (unit => unit) => unsubscribe;

  let hasPendingEffects: unit => bool;
  let runPendingEffects: unit => unit;
};

module Make:
  (
     {
      type msg;
      type model;

      let initial: model;
      let updater: Updater.t(msg, model);
      let subscriptions: model => Sub.t(msg);
    },
  ) =>
   Store;
