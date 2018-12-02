func main()
{
	Int i;
	i = 0;
}

(assert-not
	(forall ((pos Time))
		(= (i main_end) pos)
	)
)