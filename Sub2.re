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
  | Subscription(subscription('params, 'msg, 'state)): t('msg)
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

module Make = (ConfigInfo: Config) => {
  type params = ConfigInfo.params;
  type msg = ConfigInfo.msg;

  let handedOffInstance = ref(None);

  let create = params => {
    Subscription({
      handedOffInstance,
      config: (module ConfigInfo),
      params,
      state: None,
    });
  };
};

let none = NoSubscription;

// TODO:
//let batch = (subs) => SubscriptionBatch(subs);
