(* The Computer Language Benchmarks Game
 * http://benchmarksgame.alioth.debian.org/
   Contributed by Charles Martin. *)

let size = 503

let n = int_of_string Sys.argv.(1)

let run id ichan ochan =
  let rec loop () =
    let token = Event.sync (Event.receive ichan) in
    if token = n then (print_int id; print_newline (); exit 0)
    else (Event.sync (Event.send ochan (token + 1)); loop ())
  in Thread.create loop ()

let () =
  let channels =
    Array.init size
      (fun _ -> Event.new_channel ()) in
  let threads =
    Array.init size
      (fun i -> run (i + 1) channels.(i) channels.((i + 1) mod size)) in
  Event.sync (Event.send channels.(0) 0);
  Thread.join threads.(0)

