module type Provider = {
  type params;
  type msg;
  type state; // State that is carried by the subscription while it is active

  let name: string;
  let id: params => string;

  let init: (~params: params, ~dispatch: msg => unit) => state;
  let update:
    (~params: params, ~state: state, ~dispatch: msg => unit) => state;
  let dispose: (~params: params, ~state: state) => unit;
};

type provider('params, 'msg, 'state) = (module Provider with
                                           type msg = 'msg and
                                           type params = 'params and
                                           type state = 'state);

type subscription('params, 'msg, 'state) = {
  // The 'latch' serves as a gate - when the subscription is active, it will be 'true'.
  // However, once the subscription is disposed, it will be switched back to 'false'.
  // This is to handle the case where a subscription holds on to the `dispatch` function
  // - either in `update` or `init`, and tries to dispatch when the subscription is no longer available.
  latch: ref(bool),

  provider: provider('params, 'msg, 'state),
  params: 'params,
  state: option('state),
  // Borrowed from ReactMini - the "pipe" is shared between all subscriptions
  // from the same provider, allowing data to be passed between them in a
  // type-safe manner when type equality can't be proven due to `'state` being
  // an existential in when contained in `t` below
  //
  // See https://github.com/reasonml/reason-react/blob/1333211c1ea4da7be61c74084011e23137075ede/ReactMini/src/React.re#L354
  pipe: Pipe.t(option('state)),
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
      let newMapFunction = msg => f(orig(msg));
      Subscription(sub, newMapFunction);
    | SubscriptionBatch(subs) => SubscriptionBatch(List.map(map(f), subs))
    };
  };

module type S = {
  type params;
  type msg;

  let create: params => t(msg);
};

module Make = (Provider: Provider) => {
  type params = Provider.params;
  type msg = Provider.msg;

  // This "pipe" will be shared by all subscriptions originating from this provider
  let pipe = Pipe.create();

  let create = params => {
    Subscription(
      {latch: ref(false), pipe, provider: (module Provider), params, state: None},
      Fun.id,
    );
  };
};

let none = NoSubscription;
