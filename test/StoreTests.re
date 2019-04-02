open TestFramework;

open Isolinear;

type testActions =
  | Change(string);

describe("Isolinear", ({describe, _}) =>
  describe("subscribe", ({test, _}) =>
    test("state subscription fires on dispatch", ({expect}) => {
      let updater = (s, a) =>
        switch (a) {
        | Change(v) => (s ++ v, Effect.none)
        };

      let (dispatch, _) = Store.create(~initialState="hello", ~updater, ());

      let (newState, _) = dispatch(Change(" world"));

      expect.string(newState).toEqual("hello world");
    })
  )
);
