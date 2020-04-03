module State = struct

  type t = { st : int array; mutable idx : int };;

  let new_state () = { st = Array.make 55 0; idx = 0 }

  let full_init s seed =
    let combine accu x = Digest.string (accu ^ string_of_int x) in
    let extract d =
      (Char.code d.[0] + (Char.code d.[1] lsl 8) + (Char.code d.[2] lsl 16))
      lxor (Char.code d.[3] lsl 22)
    in
    let l = Array.length seed in
    for i = 0 to 54 do
      s.st.(i) <- i;
    done;
    let accu = ref "x" in
    for i = 0 to 54 + max 55 l do
      let j = i mod 55 in
      let k = i mod l in
      accu := combine !accu seed.(k);
      s.st.(j) <- s.st.(j) lxor extract !accu;
    done;
    s.idx <- 0

  (* Returns 30 random bits as an integer 0 <= x < 1073741824 *)
  let bits s =
    s.idx <- (s.idx + 1) mod 55;
    let newval = (s.st.((s.idx + 24) mod 55) + s.st.(s.idx)) land 0x3FFFFFFF in
    s.st.(s.idx) <- newval;
    newval

  let rec intaux s n =
    let r = bits s in
    let v = r mod n in
    if r - v > 0x3FFFFFFF - n + 1 then intaux s n else v

  let int s bound =
    if bound > 0x3FFFFFFF || bound <= 0
    then invalid_arg "Random.int"
    else intaux s bound

end

let def = State.new_state()
let int bound = State.int def bound
let full_init seed = State.full_init def seed
