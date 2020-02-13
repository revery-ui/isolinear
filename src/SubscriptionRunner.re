open Sub_Internal;

module Make = (RunnerConfig: {type msg;}) => {
  //type t('msg) = subscription('msg);

  type msg = RunnerConfig.msg;
  type t = Sub_Internal.t(RunnerConfig.msg);

  //let empty = NoSubscription;

  let getSubscriptionName = (subscription: t) => {
    switch (subscription) {
    | NoSubscription => "__isolinear__nosubscription__"
    | Subscription({params, state, config: (module Config)}) =>
      Config.subscriptionName ++ "$" ++ Config.getUniqueId(params)
    };
  };

  let dispose = (subscription: t) => {
    switch (subscription) {
    | NoSubscription => ()
    | Subscription({config: (module Config), params, state}) =>
      switch (state) {
      // Config was never actually created, so no need to dispose
      | None => ()
      | Some(state) => Config.dispose(~params, ~state)
      }
    };
  };

  let init = (subscription: t, dispatch: msg => unit) => {
    switch (subscription) {
    | NoSubscription => NoSubscription
    | Subscription({
        config: (module Config),
        params,
        state,
        handedOffInstance,
      }) =>
      let state = Config.start(~params, ~dispatch);

      Subscription({
        config: (module Config),
        params,
        state: Some(state),
        handedOffInstance,
      });
    };
  };

  let update = (oldSubscription, newSubscription, dispatch) => {
    switch (oldSubscription, newSubscription) {
    | (NoSubscription, NoSubscription) => NoSubscription
    | (NoSubscription, sub) => init(sub, dispatch)
    | (sub, NoSubscription) =>
      dispose(sub);
      NoSubscription;
    | (Subscription(sub1), Subscription(sub2)) =>
      let {
        config: (module ConfigOld),
        params as _oldParams,
        state as oldState,
        handedOffInstance as oldH,
      } = sub1;
      let {
        config: (module ConfigNew),
        params as newParams,
        state as _newState,
        handedOffInstance as newH,
      } = sub2;
      // We have two subscriptions that may or may not be the same type.
      // If the keys are correct, they _should_ be the same type - but getting the type system
      // to identify that is tricky! So we use the same 'handedOffInstance' trick as before.

      // We take the old instance and set its state on the old handle. If the types are the same,
      // this will be set on the new handle, since they would be pointing to the same ref.

      oldH := oldState;

      let ret =
        switch (newH^) {
        | None =>
          // Somehow... the types are different. We'll dispose of the old one, and init the new one
          dispose(oldSubscription);
          init(newSubscription, dispatch);
        | Some(oldState) =>
          // These types do match! And we know about the old state

          let newState =
            ConfigNew.update(~params=newParams, ~state=oldState, ~dispatch);
          Subscription({
            config: (module ConfigNew),
            params: newParams,
            state: Some(newState),
            handedOffInstance: newH,
          });
        };

      // We need to reset the old types now, though!
      oldH := None;
      newH := None;
      ret;
    };
  };

  let run = (~dispatch: msg => unit, ~sub: t, state: t) => {
    let oldSubscriptionName = getSubscriptionName(state);
    let newSubscriptionName = getSubscriptionName(sub);

    if (!String.equal(oldSubscriptionName, newSubscriptionName)) {
      // If subscription changed, dispose old one, and create new one
      dispose(state);
      init(sub, dispatch);
    } else {
      // Otherwise, update the current one
      update(state, sub, dispatch);
    };
  };
};
