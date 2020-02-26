type t('a) = ref(option('a));

let create = () => ref(None);

let send = (inPipe, outPipe, data) => {
  inPipe := Some(data); // send
  let received = outPipe^; // receive
  inPipe := None; // reset
  received; // return
};
