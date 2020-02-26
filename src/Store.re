type unsubscribe = unit => unit;

module type S = {
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

module Make =
       (
         Config: {
           type msg;
           type model;

           let initial: model;
           let updater: Updater.t(model, msg);
           let subscriptions: model => Sub.t(msg);
         },
       ) => {
  type msg = Config.msg;
  type model = Config.model;

  let updater = Config.updater;
  let subscriptions = Config.subscriptions;

  type t = {
    beforeMsgDispatch: msg => unit,
    beforeMsgStream: Stream.t(msg),
    afterMsgDispatch: ((msg, model)) => unit,
    afterMsgStream: Stream.t((msg, model)),
    beforeEffectDispatch: Effect.t(msg) => unit,
    beforeEffectStream: Stream.t(Effect.t(msg)),
    afterEffectDispatch: Effect.t(msg) => unit,
    afterEffectStream: Stream.t(Effect.t(msg)),
    modelChangedDispatch: model => unit,
    modelChangedStream: Stream.t(model),
    pendingEffectStream: Stream.t(unit),
    pendingEffectDispatch: unit => unit,
    latestModel: ref(model),
    pendingEffects: ref(list(Effect.t(msg))),
    updater: Updater.t(model, msg),
    // Legacy store stream for compatibility with old API
    legacyStoreDispatch: ((model, msg)) => unit,
    legacyStoreStream: Stream.t((model, msg)),
  };

  module Runner =
    SubscriptionRunner.Make({
      type msg = Config.msg;
    });
  let subscriptionState = ref(Runner.empty);

  let latestModel = ref(Config.initial);
  let pendingEffects = ref([]);
  let (modelChangedStream, modelChangedDispatch) = Stream.create();
  let (pendingEffectStream, pendingEffectDispatch) = Stream.create();
  let (legacyStoreStream, legacyStoreDispatch) = Stream.create();
  let (beforeMsgStream, beforeMsgDispatch) = Stream.create();
  let (afterMsgStream: Stream.t((msg, model)), afterMsgDispatch) =
    Stream.create();
  let (beforeEffectStream, beforeEffectDispatch) = Stream.create();
  let (afterEffectStream, afterEffectDispatch) = Stream.create();

  let store: t = {
    beforeMsgDispatch,
    beforeMsgStream,
    afterMsgDispatch,
    afterMsgStream,
    beforeEffectDispatch,
    beforeEffectStream,
    afterEffectDispatch,
    afterEffectStream,
    modelChangedStream,
    pendingEffectStream,
    modelChangedDispatch,
    pendingEffectDispatch,
    latestModel,
    pendingEffects,
    updater,
    legacyStoreDispatch,
    legacyStoreStream,
  };

  let getModel = () => store.latestModel^;

  let rec dispatch = (msg: msg) => {
    store.beforeMsgDispatch(msg);

    // BEGIN ATOMIC
    // This should be atomic, so that nested calls to [dispatch]
    // keep our store in an internally consistent state
    let currentModel = store.latestModel^;
    let (newModel, effect) = store.updater(currentModel, msg);
    let hasPendingEffect = ref(false);

    if (effect != Effect.none) {
      store.pendingEffects := [effect, ...store.pendingEffects^];
      hasPendingEffect := true;
    };

    store.latestModel := newModel;
    // END ATOMIC

    store.legacyStoreDispatch((newModel, msg));

    // In case the store was updated between states...
    let newModel = store.latestModel^;

    // Dispatch model changed event, if necessary
    if (currentModel !== newModel) {
      store.modelChangedDispatch(newModel);
    };

    if (hasPendingEffect^) {
      store.pendingEffectDispatch();
    };

    let newModel = store.latestModel^;

    // Run subscriptions
    let sub = subscriptions(newModel);

    // Defer running dispatches from subscription until after the subscription state is updated
    // Otherwise, in the case where [init] dispatches an action, we could keep calling [init]
    // since we wouldn't have recorded the new state, yet!
    let subscriptionActions: ref(list(msg)) = ref([]);
    let subscriptionLock = ref(true);
    let subscriptionDispatch = msg =>
      if (subscriptionLock^) {
        subscriptionActions := [msg, ...subscriptionActions^];
      } else {
        dispatch(msg);
      };
    subscriptionState :=
      Runner.run(~dispatch=subscriptionDispatch, ~sub, subscriptionState^);

    subscriptionActions^ |> List.rev |> List.iter(dispatch);
    subscriptionLock := false;

    store.afterMsgDispatch((msg, store.latestModel^));
  };

  let hasPendingEffects = () => store.pendingEffects^ != [];

  let runPendingEffects = () => {
    let effects = store.pendingEffects^;
    store.pendingEffects := [];

    effects
    |> List.filter(e => e != Effect.none)
    |> List.rev
    |> List.iter(e => {
         store.beforeEffectDispatch(e);
         Effect.run(e, dispatch);
         store.afterEffectDispatch(e);
       });
  };

  let onBeforeMsg = Stream.subscribe(store.beforeMsgStream);
  let onAfterMsg = subscription =>
    Stream.subscribe(store.afterMsgStream, ((model, msg)) => {
      subscription(model, msg)
    });

  let onBeforeEffectRan = Stream.subscribe(store.beforeEffectStream);
  let onAfterEffectRan = Stream.subscribe(store.afterEffectStream);

  let onModelChanged = subscription =>
    Stream.subscribe(store.modelChangedStream, subscription);

  let onPendingEffect = subscription =>
    Stream.subscribe(store.pendingEffectStream, subscription);

  module Deprecated = {
    let getStoreStream = () => {
      store.legacyStoreStream;
    };
  };
};
