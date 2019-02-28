(two-traces)

func main()
{
  const Int h;
  const Int h2;
  Int[] output;

  Int counter = 0;
  if(h > 0)
  {
    output[counter] = 5;
    counter = counter + 1;
  }
  else {
    if (h2 > 0) {
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

(assert-not
  (= (counter main_end t1) (counter main_end t2))
)

