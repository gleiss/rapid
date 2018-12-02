func main()
{
	Int i;
	i = 0;
}

(assert-not
	(forall ((pos Time))
		(= 0 (- pos))
	)
)