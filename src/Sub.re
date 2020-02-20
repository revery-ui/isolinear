module type Config = {
  type params;
  type msg;

  // State that is carried by the subscription while it is active
  type state;

  let name: string;
  let id: params => string;

  let init: (~params: params, ~dispatch: msg => unit) => state;
  let update:
    (~params: params, ~state: state, ~dispatch: msg => unit) => state;
  let dispose: (~params: params, ~state: state) => unit;
};

type config('params, 'msg, 'state) = (module Config with
                                         type msg = 'msg and
                                         type params = 'params and
                                         type state = 'state);

type subscription('params, 'msg, 'state) = {
  config: config('params, 'msg, 'state),
  params: 'params,
  state: option('state),
  // This is the same trick used in ReactMini -
  // used to avoid Obj.magic when updating subscriptions.
  handedOffInstance: ref(option('state)),
};

type t('msg) =
  | NoSubscription: t('msg)
  | Subscription(
      subscription('params, 'originalMsg, 'state),
      'originalMsg => 'msg,
    )
    : t('msg)
  | SubscriptionBatch(list(t('msg)));

let batch = subs => SubscriptionBatch(subs);

let flatten = sub => {
  let rec loop = sub => {
    switch (sub) {
    | NoSubscription => []
    | Subscription(_) as sub => [sub]
    | SubscriptionBatch(subs) => subs |> List.map(loop) |> List.flatten
    };
  };

  loop(sub);
};

let rec map: ('a => 'b, t('a)) => t('b) =
  (f, sub) => {
    switch (sub) {
    | NoSubscription => NoSubscription
    | Subscription(sub, orig) =>
      let newMapFunction = x => f(orig(x));
      Subscription(sub, newMapFunction);
    | SubscriptionBatch(subs) => SubscriptionBatch(List.map(map(f), subs))
    };
  };

module type S = {
  type params;
  type msg;

  let create: params => t(msg);
};

module Make = (ConfigInfo: Config) => {
  type params = ConfigInfo.params;
  type msg = ConfigInfo.msg;

  let handedOffInstance = ref(None);

  let mapper = a => a;

  let create = params => {
    Subscription(
      {handedOffInstance, config: (module ConfigInfo), params, state: None},
      mapper,
    );
  };
};

let none = NoSubscription;
