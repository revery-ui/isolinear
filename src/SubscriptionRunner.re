open Sub;

module Make = (RunnerConfig: {type msg;}) => {
  //type t('msg) = subscription('msg);

  type msg = RunnerConfig.msg;
  type t = Hashtbl.t(string, Sub.t(RunnerConfig.msg));

  let empty: Hashtbl.t(string, Sub.t(RunnerConfig.msg)) = Hashtbl.create(0);

  let getSubscriptionName = (subscription: Sub.t(msg)) => {
    switch (subscription) {
    | NoSubscription => "__isolinear__nosubscription__"
    | Subscription({params, state, config: (module Config)}) =>
      Config.subscriptionName ++ "$" ++ Config.getUniqueId(params)
    | SubscriptionBatch(_) => "__isolinear__batch__"
    };
  };

  let dispose = (subscription: Sub.t(msg)) => {
    switch (subscription) {
    | NoSubscription => ()
    | Subscription({config: (module Config), params, state}) =>
      switch (state) {
      // Config was never actually created, so no need to dispose
      | None => ()
      | Some(state) => Config.dispose(~params, ~state)
      }
    // This should never be hit, because the batches are removed
    // prior to reconciliation
    | SubscriptionBatch(_) => ();
    };
  };

  let init = (subscription: Sub.t(msg), dispatch: msg => unit) => {
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
    // This should never be hit
    | SubscriptionBatch(_) => NoSubscription
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
      // to identify that is tricky! So we use the same 'handedOffInstance' trick as before:t
      // https://github.com/reasonml/reason-react/blob/1333211c1ea4da7be61c74084011e23137075ede/ReactMini/src/React.re#L354

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
    // Subscription batch case - should not be hit
    | _ => NoSubscription
    }
  };

  let reconcile = (subs, oldState, dispatch) => {
  
    let newState = Hashtbl.create(Hashtbl.length(oldState));
    let iter = (sub: Sub.t(msg)) => {
       let subscriptionName = getSubscriptionName(sub);

       // Is this a new subscription, or a previous one?
       let newSubState = switch (Hashtbl.find_opt(oldState, subscriptionName)) {
       // New subscription - we'll init it
       | None => init(sub, dispatch)

       // Existing subscription - we'll update it
       | Some(previousSub) => update(previousSub, sub, dispatch)
       }

       Hashtbl.replace(newState, subscriptionName, newSubState);
    };

    List.iter(iter, subs);

    newState;
  };

  let run = (~dispatch: msg => unit, ~sub: Sub.t(msg), state: t) => {

    let subs = Sub.flatten(sub);
    let newState = reconcile(subs, state, dispatch);
    
    // Diff the old state, and the new state, and see which subs
    // were removed
    Hashtbl.iter((key, v) => {
    
      switch (Hashtbl.find_opt(newState, key)) {

      // The subscription is still around, so nothing to do.
      | Some(_) => ()
      
      // The subscription was removed from the old state,
      // so we need to dispose of it
      | None =>
      dispose(v);
      }
  
    }, state);

    newState;

  };
};
