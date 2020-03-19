(set-traces 2)

func main()
{
  const Int h;
  const Int h2;
  Int[] output;

  Int counter = 0;
  if(h == 0)
  {
    output[counter] = 5;
    counter = counter + 1;
  }
  else {
    if (h2 == 0) {
      output[counter] = 5;
      counter = counter + 1;
    } else {
      output[counter] = 5;
      counter = counter + 1;
    }
  }
  output[counter] = 7;
  counter = counter + 1;
}

(conjecture
  (= (counter main_end t1) (counter main_end t2))
)

(conjecture
  (=>
    (and
      (forall ((pos Int))
        (= (output l9 pos t1) (output l9 pos t2))
      )
    )
    (forall ((pos Int))
      (= (output main_end pos t1) (output main_end pos t2))
    )
  )
)
