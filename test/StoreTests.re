open TestFramework;


type testActions =
| Change(string);


describe("Isolinear", ({describe, _}) => {

    describe("subscribe", ({test, _}) => {

        test("state subscription fires on dispatch", ({expect}) => {
            open Isolinear;
            let updater = (s, a) => switch (a) {
            | Change(v) => ((s ++v), Isolinear_Types.Effect.none)
            };

            module MyStore = Isolinear.Store.Make({
                type state = string;
                type action = testActions;
                let initialState = "hello";
                let updater = updater;
            })

            let lastValue = ref("");

            let _ = MyStore.subscribe((v) => {
                lastValue := v;  
            });

            MyStore.dispatch(Change(" world"));

            expect.string(lastValue^).toEqual("hello world");
        });
        
    });
});
