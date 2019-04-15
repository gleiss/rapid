(two-traces)

func main()
{
  const Int h;
  Int[] output;
  Int l = 5;

  Int counter = 0;
  

  if (h == 1) {
    l = l - 1;
  }
  else {
    skip;
  }

  l = 4;
  output[counter] = l;
  counter = counter + 1;
}

(conjecture
  (and
    (= (counter main_end t1) (counter main_end t2))
    (forall ((i Int) (tr Trace))
      (=>
        (and
          (< i (counter main_end tr))
          (<= 0 i)
        )
        (= (output main_end i t1) (output main_end i t2))
      )
    )
  )
)

