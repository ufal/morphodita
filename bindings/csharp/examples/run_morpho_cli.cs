using

Morpho morpho = Morpho.load(args[0]);
  ...
  TaggedLemmas lemmas = new TaggedLemmas();
  morpho.analyze("čau", Morpho.GUESSER, lemmas);
  foreach (TaggedLemma lemma in lemmas)
    Console.WriteLine("{0} - {1}", lemma.lemma, lemma.tag);


