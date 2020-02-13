open TestFramework;

open Isolinear;

module SubscriptionRunner = Isolinear.Internal.SubscriptionRunner;

type testState =
  | Init(int)
  | Update(int)
  | Dispose(int);

let disposeState: ref(list(testState)) = ref([]);

module Runner =
  SubscriptionRunner.Make({
    type msg = testState;
  });

module TestSubscription =
  Sub.Make({
    type params = int;
    type msg = testState;

    type state = list(testState);

    let subscriptionName = "TestSubscription";

    let getUniqueId = params => params |> string_of_int;

    let start = (~params, ~dispatch) => {
      dispatch(Init(params));
      [Init(params)];
    };

    let update = (~params, ~state, ~dispatch) => {
      let newState = [Update(params), ...state];
      dispatch(Update(params));
      newState;
    };

    let dispose = (~params as _, ~state as _) => {
      ();
    };
  });

describe("SubscriptionRunner", ({describe, _}) =>
  describe("subscribe", ({test, _}) => {
    test("init is called", ({expect, _}) => {
      let lastAction: ref(option(testState)) = ref(None);
      let dispatch = action => {
        lastAction := Some(action);
      };
      let sub = TestSubscription.create(1);
      let _ = Runner.run(~dispatch, ~sub, Runner.empty);

      expect.equal(lastAction^, Some(Init(1)));
    });

    test("update is called", ({expect, _}) => {
      let lastAction: ref(option(testState)) = ref(None);
      let dispatch = action => {
        lastAction := Some(action);
      };
      let sub = TestSubscription.create(1);
      let newState = Runner.run(~dispatch, ~sub, Runner.empty);
      let _ = Runner.run(~dispatch, ~sub, newState);

      expect.equal(lastAction^, Some(Update(1)));
    });
  })
);
