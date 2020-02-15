open TestFramework;

open Isolinear;

type testActions =
  | Change(string);

let startCount = ref(0);
let updateCount = ref(0);
let disposeCount = ref(0);
module TestSubscription =
  Sub.Make({
    type params = int;
    type msg = bool;

    type state = unit;

    let subscriptionName = "testSubscription";
    let getUniqueId = _ => "unique";

    let start = (~params as _, ~dispatch as _) => {
      incr(startCount);
      ();
    };

    let update = (~params as _, ~state as _, ~dispatch as _) => {
      incr(updateCount);
      ();
    };

    let dispose = (~params as _, ~state as _) => {
      incr(disposeCount);
      ();
    };
  });

describe("Store", ({describe, _}) =>
  describe("subscriptions", ({test, _}) => {
    test("init / update / dispose get called", ({expect, _}) => {
      let sub = TestSubscription.create(1);
      module TestStore =
        Store.Make({
          type msg = bool;
          type model = bool;

          let initial = false;
          let updater = (_model, msg) => (msg, Effect.none);
          let subscriptions = model => model ? sub : Sub.none;
        });

      startCount := 0;
      updateCount := 0;
      disposeCount := 0;

      TestStore.dispatch(true);
      expect.equal(startCount^, 1);
      expect.equal(updateCount^, 0);
      expect.equal(disposeCount^, 0);

      TestStore.dispatch(true);
      expect.equal(startCount^, 1);
      expect.equal(updateCount^, 1);
      expect.equal(disposeCount^, 0);

      TestStore.dispatch(false);
      expect.equal(startCount^, 1);
      expect.equal(updateCount^, 1);
      expect.equal(disposeCount^, 1);
    })
  })
);
/*test("state subscription fires on dispatch", ({expect, _}) => {
      let updater = (s, a) =>
        switch (a) {
        | Change(v) => (s ++ v, Effect.none)
        };

      let (dispatch, _) = Store.create(~initialState="hello", ~updater, ());

      let (newState, _) = dispatch(Change(" world"));

      expect.string(newState).toEqual("hello world");
    })
  )*/
