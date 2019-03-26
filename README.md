[![Build Status](https://bryphe.visualstudio.com/isolinear/_apis/build/status/bryphe.isolinear?branchName=master)](https://bryphe.visualstudio.com/isolinear/_build/latest?definitionId=22&branchName=master)

# isolinear

Experimental state management framework for native Reason

### Introduction

`isolinear` is a [Redux](https://reduxjs.org) and [Elm Architecture](https://guide.elm-lang.org/architecture/) inspired framework. 

It provides a functor for creating an application model, and _updating_ the model. Like Elm, `Update`s specify both a way to update the state, and side effects.

### FAQ

- __Why a central store, if you have `useReducer` hook anyway?__

Having a centralized model, _external_ to the UI, allows for some powerful capabilities:
- Model updates, if the store is immutable, can be run in parallel to the UI (in a separate OS thread)
- Model can be tested completely independently of the UI

### License

[MIT License](./LICENSE)
