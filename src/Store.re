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

module Make =
       (
         Config: {
           type msg;
           type model;

           let initial: model;
           let updater: Updater.t(msg, model);
           let subscriptions: model => Sub.t(msg);
         },
       ) => {
  type msg = Config.msg;
  type model = Config.model;

  let updater = Config.updater;
  let subscriptions = Config.subscriptions;

  type t = {
    modelChangedDispatch: model => unit,
    modelChangedStream: Stream.t(model),
    pendingEffectStream: Stream.t(unit),
    pendingEffectDispatch: unit => unit,
    latestModel: ref(model),
    pendingEffects: ref(list(Effect.t(msg))),
    updater: Updater.t(msg, model),
    //subscriptions: ('model) => Sub.t('msg),
  };

  module Runner = SubscriptionRunner.Make({
    type msg = Config.msg;
  });
  let subscriptionState = ref(Runner.empty);

  let latestModel = ref(Config.initial);
  let pendingEffects = ref([]);
  let (modelChangedStream, modelChangedDispatch) = Stream.create();
  let (pendingEffectStream, pendingEffectDispatch) = Stream.create();

  let store: t = {
    modelChangedStream,
    pendingEffectStream,
    modelChangedDispatch,
    pendingEffectDispatch,
    latestModel,
    pendingEffects,
    updater,
  };

  type unsubscribe = unit => unit;

  let getModel = () => store.latestModel^;

  let rec dispatch = (msg: msg) => {
    let currentModel = store.latestModel^;
    let (newModel, effect) = store.updater(currentModel, msg);
    let hasPendingEffect = ref(false);

    if (effect != Effect.none) {
      store.pendingEffects := [effect, ...store.pendingEffects^];
      hasPendingEffect := true;
    };

    store.latestModel := newModel;

    // Dispatch model changed event, if necessary
    if (currentModel !== newModel) {
      store.modelChangedDispatch(newModel);
    };

    if (hasPendingEffect^) {
      store.pendingEffectDispatch();
    };

    // Run subscriptions
    let sub = subscriptions(newModel);
    subscriptionState := Runner.run(~dispatch, ~sub, subscriptionState^);
  };

  let hasPendingEffects = () => store.pendingEffects^ != [];

  let runPendingEffects = () => {
    let effects = store.pendingEffects^;
    store.pendingEffects := [];

    effects
    |> List.filter(e => e != Effect.none)
    |> List.rev
    |> List.iter(e => Effect.run(e, dispatch));
  };

  let onModelChanged = subscription => {
    Stream.subscribe(store.modelChangedStream, subscription);
  };

  let onPendingEffect = subscription => {
    Stream.subscribe(store.pendingEffectStream, subscription);
  };
};
