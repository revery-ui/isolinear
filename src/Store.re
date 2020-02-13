type t('msg, 'model) = {
  modelChangedDispatch: 'model => unit,
  modelChangedStream: Stream.t('model),
  pendingEffectStream: Stream.t(unit),
  pendingEffectDispatch: unit => unit,
  latestModel: ref('model),
  pendingEffects: ref(list(Effect.t('msg))),
  updater: Updater.t('msg, 'model),
  //subscriptions: ('model) => Sub.t('msg),
};

type unsubscribe = unit => unit;

let create =
    //~subscriptions=(_) => Sub.none,
    (~updater, model) => {
  let latestModel = ref(model);
  let pendingEffects = ref([]);

  let (modelChangedStream, modelChangedDispatch) = Stream.create();
  let (pendingEffectStream, pendingEffectDispatch) = Stream.create();

  {
    /*subscriptions,*/ modelChangedStream,
    pendingEffectStream,
    modelChangedDispatch,
    pendingEffectDispatch,
    latestModel,
    pendingEffects,
    updater,
  };
};

let getModel = ({latestModel, _}) => latestModel^;

let dispatch = (store: t('msg, 'model), msg: 'msg) => {
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
};

let hasPendingEffects = ({pendingEffects, _}) => pendingEffects^ != [];

let runPendingEffects = store => {
  let effects = store.pendingEffects^;
  store.pendingEffects := [];

  effects
  |> List.filter(e => e != Effect.none)
  |> List.rev
  |> List.iter(e => Effect.run(e, dispatch(store)));
};

let onModelChanged = (store, subscription) => {
  Stream.subscribe(store.modelChangedStream, subscription);
};

let onPendingEffect = (store, subscription) => {
  Stream.subscribe(store.pendingEffectStream, subscription);
};
