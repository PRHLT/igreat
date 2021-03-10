# iGREAT
iGREAT is an open-source, statistical machine translation software toolkit based on finite-state models.


## Table of contents
* [Package Contents](#package-contents).
* [How to Build iGREAT](#how-to-build-igreat).
* [How to Run iGREAT](#how-to-run-igreat).
* [Usage Details](#usage-details).
* [File Formats](#file-formats).
* [Documentation](#documentation).
* [Troubleshooting](#troubleshooting).
* [Literature](#literature).
* [History](#history).

## Package Contents

The main functionality of iGREAT is implemented by two programs:

* `giati`: a tool to train an SFST from a word-aligned parallel corpus.
* `refx`:	 a tool to translate a test corpus by means of a trained SFST.

Additionally, iGREAT includes two more tools for a log-linear approach to SMT:

* `arpa2tr`: a tool to turn ARPA language models into SFSTs.
* `local`: several phrase-based local features are computed for a given set of bilingual phrase pairs: phrase-based lexical probabilities (direct and inverse lexical translation models) and a target-sentence length model.
* `simplex`: a tool to train the set of weights within a log-linear approach.

## How to Build iGREAT

 1. `cd` to the directory containing iGREAT and type `./configure` to configure the package for your system.

 2. Type `make` to compile the package.

 3. Type `make install` to install the programs and any data files and documentation.

By default the files are installed under the `/usr/local/` directory (or
similar, depending on the OS you use); however, since the installation
step requires root privileges, another directory can be specified in the
first step of the installation by typing:

```
$ ./configure --prefix=<absolute-installation-path>
```

For example, if `user1` wants to install iGREAT in the directory `/home/user1/iGREAT`, the sequence of commands to execute should be the following:

```
	$ ./configure --prefix=/home/user1/iGREAT
	$ make
	$ make install
```

NOTE: the installation directory can be the same directory where iGREAT
was decompressed.

See the *INSTALL* file for more information.

## How to Run iGREAT

### A. How to train an SFST from a word-aligned parallel corpus
The tool to train a joint probability model with iGREAT is named `giati`
and it basically performs in three sequential steps:

(1) A string set is extracted from the word-aligned training corpus
   based on the concept of monotonic bilingual sentence segmentation.

(2) An n-gram model is trained from the string set obtained in step 1.

(3) Finally, an SFST is built from the n-gram model trained in step 2.

A configuration file must be always provided together with `giati`:

```
$ giati -c <config_file>
```

This file points to several working directories and establishes the value of
some training parameters. Its content has to follow this text format:

```
CORPUSDIR = /home/jgonza/contests/iwslt2010/corpus/talk
CORPUS = training
ALIGN = training.align
TEST = test
SEPARATOR = _#_

WORKDIR = /home/jgonza/contests/iwslt2010/corpus/work
TMPDIR = /home/jgonza/contests/iwslt2010/corpus/tmp
SRILMDIR = /home/jgonza/tools/srilm/bin/i686

ALGORITHM = 3
SIMBEXTMAX = 7

NGRAM = 4

PREFIX = n4.
OUTPUT = v2

HASH = 4194304
```

where corpora are expected to be organized under `CORPUSDIR` directory; `CORPUS`,
`ALIGN`, and `TEST` entries refer to the filenames of, respectively, the training
corpus, the corresponding alignments, and the test corpus. The training and
the test corpora are raw text files where every line contains a source
sentence, a `SEPARATOR` string, and a target sentence. The test corpus is only
provided for filtering the SFSTs being inferred. The alignments file follows
the GIZA++ format and it is used for extracting monotonic bilingual string
segmentations from the parallel training corpus in order to build a string set.

The (source-to-target) alignments file can be estimated by GIZA++ itself.

Three more directories have to be specified: `WORKDIR`, `TMPDIR`, and `SRILMDIR`.
Whereas `WORKDIR` and `TMPDIR` are different working directories of iGREAT,
`SRILMDIR` contains the SRILM binary `ngram-count` in order to be able to compute
n-gram models.

Next, several training parameters can be instanced in order to provide
different configurations for the translation models being inferred. For
example, `ALGORITHM` and `SIMBEXTMAX` refer to the way the alignments are
processed to extract a string set from the parallel corpus (step 1)

`ALGORITHM` determines the extraction algorithm to be used. So far,
there are only two bilingual segmentation algorithms implemented,
one for a word-based and one for a phrase-based SMT approach
They are selected by setting this parameter to 1 or 3, respectively.

`SIMBEXTMAX` limits the number of words in the aligned segments.
For example, for the sentences *El coche rojo* and *The red car*,
a bilingual string like *El\*The coche-rojo\*red-car* can be built
Two bilingual symbols are used, *El\*The* and *coche-rojo\*red-car*,
which derive from aligned segments of 2 and 4 words, respectively.

The `NGRAM` value refers to the n-gram order that is requested to train (step 2).

Finally, `PREFIX` and `OUTPUT` refer to output file formats and names. The
parameter `PREFIX` is a filename prefix for all the files that will be generated
during training. The model format is selected with the `OUTPUT` parameter:
`arpa` for the n-gram model or `v2` for the SFST. If `arpa` is selected, then
step 3 of `giati` is actually skipped and the translation model is expressed
as an n-gram model of bilingual symbols. Should it is `v2`, an SFST will be
built from the n-gram model (step 3).

Memory issues are dealt with the proposed value in the `HASH` parameter.

Apart from that, there are some other command-line options that overwrite the
settings in the configuration file. Moreover, some additional parameters can
only be specified through the command-line interface. That is the case of
several training flags and filter options:

```
--end		If the --end flag is included, then training is not actually
		performed, but however the control characters needed for step 1
		are reported. That makes sense if step 1 of `giati' is required
		to be skipped because an external bilingual string corpus will
		be provided later, instead of using the embedded segmentation
		algorithms of iGREAT. This alternative file has to follow anyway
		the same format and so the key characters reported with --end
		are needed to build it.

--segment	The --segment flag skips step 1 of `giati', thus starting it
		from step 2 using the presumedly provided file.

-F <integer>    It deals with filtering the SFST building by means of the test
                corpus. This technique implements a sliding window of the size
                indicated by `-F' to determine if any trained n-gram represents
                an SFST reachable path by some of the source sentences that are
                going to be translated later. If not succeeded, then they are
                not represented in the SFST structure. The greater the window
                size is, the more constrained is reachability, and therefore
                the smaller SFSTs are (until convergence is achieved).
                By default, `-F' is set to 0, which means no filtering at all.
```

Both the resulting n-gram and SFST models are stored in `CORPUSDIR` directory.

### B. How to translate a test corpus by means of a trained SFST
The binary to translate a test corpus with iGREAT is called `refx`
and the model must also be provided:

```
	$ refx -m <model> -t <test>
```

With respect to the `-m <model>` option, file extensions, i.e. `.tr` for SFSTs
that are inferred by means of the `giati` tool, must be omitted.

Test corpora are expected to be raw text files where every line contains a
source sentence, a separator string, and a target sentence, as usual. In
this case, target sentences are only provided to also output them together
with the system hypotheses for posterior evaluation purposes. The separator
string is then indicated to `refx` by means of the `-s` option. For example,
if *\_#\_* is used as string separator, then `refx` must be invoked this way:

```
	$ refx -m <model> -t <test> -s '_#_'
```

Additional running options can be introduced via command-line too:

```
-b <integer>	It triggers a beam-search technique based on histogram pruning.
		In general terms, it means that only the number indicated by -b
		of partial hypotheses are taken into account in the trellis
		from stage to stage, i.e. the best scored ones. The lower this
		number is, the more constrained the search process is, and
		therefore it is faster. However, translation results are also
		very poor if a too reduced number is considered. As a
		consequence, a trade-off between quality and time requirements
		is usually empirically determined in order to reduce the
		response time without a significant quality loss.

--smooth	Backoff edges are not longer treated as failure transitions.
		Instead, they are bypassed after exhausting all possible
		transition options labelled with any pair of strings. Then,
		other possible transitions using different string pairs are
		explored.
```

Translations are printed by standard output in the format defined in IV.j.

### C. How to model a log-linear approach for phrase-based SFSTs
A log-linear approach for phrase-based SFSTs is modelled in 4 steps:

#### 1. Selection of phrase-based local features
Phrase-based SFSTs that are inferred by means of the `giati` tool
can be extended by means of several phrase-based local features.

The `local` tool allows users to define three phrase-based local features
to be used in combination with a main phrase-based SFST trained with `giati`:

- An inverse tranlation model.
- A direct translation model.
- A target-sentence length model.

The usage of `local` is similar to `refx` as for their running options.
Nevertheless, there are two of them that are particularly needed here:
on the one hand, `-f 2` selects the n-gram (and not the SFST) model,
where a `.vocabext` file is containing the set of bilingual phrase pairs
(this file is created during training along with the `.arpa` n-gram file);
on the other hand, `-C 'XY'` specifies the two `X` and `Y` control characters
that were used to build the language model vocabulary during training.

```
$ local -m <model> -t <test> -f 2 -C 'XY'
```

With respect to the `-m <model>` option, file extensions, i.e. `.arpa`
for n-gram models trained by means of `giati`, must be omitted.

The control characters for a task are reported to the user by doing:

```
$ giati -c <config_file> --end X(88) Y(89)
```

Here, these characters are `X` and `Y`, whose ASCII codes are also reported.
Then, they are applied to `local` in this manner:

```
$ local -m <model> -t <test> -f 2 -C 'XY'
```

A `.lex` file containing a word-based statistical dictionary in both directions
is also needed by `local`. This file can be automatically built by means of
the `merge_t3.awk` script, available at the `scripts` directory of iGREAT.
Its usage is:

```
$ merge_t3.awk <trg2src.t3> <src2trg.t3> <src.vcb> <trg.vcb>
```

where all four arguments are files that can be computed by means of GIZA++:

```
<trg2src.t3>	target-to-source `GIZA++.t3.final' output file from GIZA++
<src2trg.t3>	source-to-target `GIZA++.t3.final' output file from GIZA++
<src.vcb>       source vocabulary as defined by the `plain2snt.out' binary
<trg.vcb>       target vocabulary as defined by the `plain2snt.out' binary
```

After execution, a `.probs` file is created from the `.vocabext` file
where the former extends the latter with three scores per phrase pair.
For later use, the `.probs` file has to be renamed to the `.vocabext` file.

Users may use these features or not and also define others of their own
interest by following the format in the `.probs` file provided by `local`.

#### 2. Building an extended SFST
All the local scores in the `.vocabext` file are embedded into the main SFST
by means of an extended SFST with multiple scores per transition.

An extended SFST is built by `arpa2tr` and, for this purpose, its basic usage is:

```
	$ arpa2tr -m <model> -C 'XY' -n M
```

where `M` refers to the number of scores per transition to deal with.
For example, if `local` features are used, then `M` is instanced to 4:

```
	$ arpa2tr -m <model> -C 'XY' -n 4
```

as three scores are added to the original n-gram probability in the main SFST.

The extended SFST overwrites (if already existed) the `<model>.tr` file.

#### 3. Training the set of log-linear weights
The tool to train the set of weights in a log-linear approach is named `simplex`
and it uses the same syntax as `refx` as decoding tool:

```
	$ simplex -m <model> -t <development> -s '_#_' --smooth -n 4
```

where `<model>` is the extended SFST built one step above by means of `arpa2tr`
and `<development>` is expected to follow the same format as the test set.
The number of models to be considered is given by `-n`, which is 4 in case that
all scores in the extended SFST are employed in the log-linear approach.

As usual, the file extension of the model (`.tr`) is omitted.

Iterative results are printed by standard output in the format defined in [input files; (d)](#input-files)
After convergence is achieved, their final values are shown.

#### 4. Building an integrated SFST
An integrated SFST to model the whole log-linear approach is built by `arpa2tr`
by means of the set of weights estimated in the previous step. They are
instanced by means of the following running options:

```
-g <float>	weight for the joint probability model (the main SFST)
-i <float>	weight for the inverse translation model
-d <float>	weight for the direct translation model
-l <float>	weight for the target-sentence length model
```

The number of scores per transition must be set to 1 (`-n 1`)
so that all the scores in a transition are recombined into a global score
by means of computing the log-linear combination over scores and weights.

The integrated SFST keeps the structure of the main SFST,
where only the scores originally estimated by `giati` are modified.
For the example above, the `arpa2tr` command line is:

```
	$ arpa2tr -m <model> -C 'XY' -n 1 -g 0.433313 -i 0.098254 -d 0.175745 -l 0.292688
```

The integrated SFST overwrites (if already existed) the `<model>.tr` file.
This model can be used to translate a test corpus as already shown in [how to run iGREAT; B](#b.-how-to-translate-a-test-corpus-by-means-of-a-trained-sfst).

## File Formats
iGREAT works with different alignment and model file formats. In this
section, these formats are described within the context they are used.

### Training an SFST for a joint probability model (giati)

#### INPUT FILES

**(a) Training corpus format**

Corpora are collected under raw text files where every line contains
a source sentence, a separator string, and a target sentence. For example:

```
La casa verde . _#_ The green house .
```

**(b) GIZA++ alignment format**

Here is an example of an alignment in GIZA++ format for a sentence pair
(extracted from the README file provided in the GIZA++ package):

```
# Sentence pair (1)
La casa verde .
NULL ({ }) The ({ 1 }) green ({ 3 }) house ({ 2 }) . ({ 4 })
```

The alignment file is represented by three lines for each sentence
pair. The first line is a label that can be used, e.g., as a caption for
alignment visualization tools. It contains information about the
sentence sequential number in the training corpus, sentence lengths, and
alignment probability. The second line is the target sentence, the third
line is the source sentence. Each token in the source sentence is
followed by a set of zero or more numbers. These numbers represent the
positions of the target words to which this source word is connected,
according to the alignment.

#### OUTPUT FILES

**(c) String set extracted from the word-aligned training corpus based on the
    concept of monotonic bilingual sentence segmentation (`.compost`).**

The string file is represented by (at least) one line per sentence pair.
Each line is an extended-symbol string where a symbol is any character sequence
with no blank character. Blanks are then useful to identify symbol boundaries.

Each symbol is formatted as follows:

```
	source_1Ysource_2Y...Ysource_uXtarget_1Xtarget_2X...Xtarget_v
```

assuming that characters `X` and `Y` are selected as control characters.

Each symbol represents an alignment at segment level between the source
and the target sentences. Words in a source segment are encoded to be later
distinguished by means of the second control character, i.e. `Y` in the example.
Words in a target segment are similarly encoded by using the first control
character, i.e. `X`, which is also used to mark where the source segment ends
and the target segment starts.

For the example above, the following string of 3 symbols may be extracted:

```
TheXLa greenYhouseXcasaXverde .X.
```

This file is created by `giati` within its step 1
within the WORKDIR directory defined in its configuration file
with the extension `.compost`.

**(d) Codification of the symbols in the string set built in (c) (`.vocabext`)**

The vocabulary of **(c)** is encoded and each different symbol is given a number.
The format of this file is shown by means of our previous example:

```
1 TheXLa
2 greenYhouseXcasaXverde
3 .X.
...
```

This file is created by `giati` within its step 2
within the CORPUSDIR directory defined in its configuration file
with the extension `.vocabext`.

**(e) Codification of the string set extracted in (c) (`.compostcod`)**

The string set extracted in **(c)** is encoded according to **(d)**
(every symbol in **(c)** is replaced by its given number in **(d)**).
Therefore, the format of this file is as follows:

```
1 2 3
...
```

This file is created by `giati` within its step 2
within the TMPDIR directory defined in its configuration file
with the extension `.compostcod`.

**(f) n-gram file format (`.arpa`)**

iGREAT uses standard ARPA format for n-gram backoff language models,
which is described, for example, at:

```
http://www-speech.sri.com/projects/srilm/manpages/ngram-format.5.html
```

This file is created by `giati` within its step 2
within the CORPUSDIR directory defined in its configuration file
with the extension `.arpa`.

**(g) SFST format (`.tr`)**

Here is an example of an SFST for an English-to-French translation task:

```
Name 3-grams_transducer

NumStates 809588


State 0 i = 0.0000 f = 0.3514
0 1 "i" p = 0.0578 o = "j'"
0 100 "of" p = 0.0255 o = "du"
0 100006 "larger" p = 0.0007 o = "de plus gros"
...
```

This format starts with a header, introduced by the keyword `Name` followed by
the name of the SFST, and the keyword `NumStates` followed by the number of
states in the model. After that, states are described in numerical order,
starting by state number 0:

- First, initial and final probabilities for the corresponding state are shown:
```
State 0 i = 0.0000 f = 0.3514
```

which means that `state 0` is not an initial state and that its final probability
is `0.3514`.

- then, all its outgoing transitions, e.g. coming from state 0, are described:
```
0 1 "i" p = 0.0578 o = "j'"
```

which means that this transition goes from `state 0` to `state 1`, its input label
is the word `i`, its output string is *"j'"*, and its probability is *0.0578*.

### Translate a test corpus by means of a trained SFST (`refx`)

#### INPUT FILES
**(h) Test corpus format**

The same as described in Section IV.a for training corpora.

#### OUTPUT FILES
**(i) Standard output**

`refx` translates a test set as in the following examples:

```
 1:  thank you very much . _#_ merci beaucoup .
-1:  we need to go far , quickly _#_ nous avons besoin d' aller
     loin , et vite _#_ nous avons besoin aller loin , rapidement
```

which is formatted as follows:

```
" 1:"  source sentence + translation hypothesis

for translations matching completely the target reference, and

"-1:"  source sentence + target reference + translation hypothesis

for translations NOT matching completely the target reference.

The symbol `+' stands for the string separator found in the test corpus.
```

### Training an SFST for a log-linear approach of phrase-based SFSTs

#### Building the lexicon file (`.lex`) needed by `local`


##### INPUT FILES
**(j) GIZA++ vocabulary format (`.vcb`)**

Vocabulary is encoded and each different word is given a number starting by 2.
Frequency counts throughout all the training corpus are also included.
Each word is stored on one line as follows:
```
 2 wordA no_occurrencesA
 3 wordB no_occurrencesB
 4 wordC no_occurrencesC
 ....
 ```

Here is an example from an English vocabulary file:
```
2 i 11849
3 grew 46
4 up 1672
...
```

**(k) GIZA++ translation format (`*.t3.*`)**

Each line is of the following format:
```
s_id t_id Pr(t_id|s_id)
```

where:
 `s_id`is the id for the source word according to the vocabulary file (`.vcb`).
 `t_id` is the id for the target word according to the vocabulary file (`.vcb`).
 `Pr(t_id|s_id)` is the probability of translating `s_id` as `t_id`.

Sample part of a file:
```
261 4134 0.00389947
24416 3323 0.249218
3558 9 0.310349
5179 9510 0.0676508
```

##### OUTPUT FILES
**(l) Word-based statistical dictionary in both directions (`.lex`)**

This file is formatted as follows:
```
        s_word   t_word   Pr(s_word|t_word)   Pr(t_word|s_word)
```

where word pairs are not expected to be listed under any special order.
For example, a line such as:
```
        great grande 0.223609 0.137312
```

means that `Pr(great|grande) = 0.223609` and `Pr(grande|great) = 0.137312`.

#### Computation of phrase-based local features (`local`)
All input files are already described above.

##### OUTPUT FILES
**(m) Phrase-based local features (`.probs`)**

This file is an extension of **(d)** that includes several features per symbol.
Features are modelling the aligned segments that are behind the symbols.

Each line is of the following format:
```
id symbol Pr(s_segment|t_segment) Pr(t_segment|s_segment) |t_segment|
```

where `symbol` actually represents a source and a target segment, encoded by
means of the above-said control characters (see [input files; (c)](#input-files) for more information).

Here is an example:
```
1 TheXLa 0.000102373 0.10943 1
2 greenYhouseXcasaXverde 0.0955086 0.0148509 2
3 .X.  0.347402 0.113469 1
...
```

which, e.g. for the second line, means that:
```
- Pr("green house"|"casa verde") = 0.0955086
- Pr("casa verde"|"green house") = 0.0148509
- |"casa verde"| = 2 (number of words in the target segment "casa verde")
```

#### Building an extended SFST
All input files are already described above.

##### OUTPUT FILES
**(n) Extended SFST format**

Extended SFSTs follow the same format as standard SFSTs ([input files; (g)](#input-files)) except that line transitions are extended by means of extra transition scores.
For example:
```
0 1 "i" p = 0.0578 p = 0.3474 p = 0.1134 p = 1 o = "j'"
0 100 "of" p = 0.0255 p = 0.1747 p = 0.0271 p = 1 o = "du"
0 100006 "larger" p = 0.0007 p = 0.0056 p = 0.0005 p = 3 o = "de plus gros"
```

where the first `p = score` pattern refers to original transition probabilities
and the remainder are related to the new phrase-based local features included,
namely:

* the second one refers to the inverse translation model,
* the third one, to the direct translation model,
* and the fourth one, to the target-sentence length model.

#### Training the set of log-linear weights

##### INPUT FILES
**(o) Development corpus format**

The same as described in [input files; (a)](#input-files) for training corpora.

##### OUTPUT FILES
**(p) Standard output**

Iterative results are printed by standard output. After convergence is achieved,
their final values are shown. Here is an example of the last output lines:
```
0.433313 0.098254 0.175745 0.292688 ::: BLEU = 0.196798
0.410406 0.092877 0.156622 0.340094 ::: BLEU = 0.196347
0.432583 0.087261 0.160632 0.319523 ::: BLEU = 0.196453
0.430985 0.096003 0.152672 0.320340 ::: BLEU = 0.196293
0.421534 0.102972 0.168107 0.307387 ::: BLEU = 0.196791
```

which means that the best score (BLEU = 0.196798) is obtained by means of the
corresponding log-linear weights shown in the left part of that line, namely:
```
0.433313 as the weight for the joint probability model (the main SFST),
0.098254 as the weight for the inverse translation model,
0.175745 as the weight for the direct translation model, and
0.292688 as the weight for the target-sentence length model.
```

#### Building an integrated SFST
All input files are already described above.

##### OUTPUT FILES
**(q) Integrated SFST**

Integrated SFSTs follow the same format as standard SFSTs ([input files; (g)](#input-files)).

## Documentation
Available documentation for iGREAT:

- This README file.

- All the work that iGREAT is based on is described in (González, 2009).

- A general description of the toolkit can be found in
  (González and Casacuberta, 2009).

- On the use of iGREAT in conjunction with other SMT systems is presented
  in (González et al., 2008).

- A preliminary study on Factored SFSTs is carried out in
  (González and Casacuberta, 2008).

- The performance of word-based SFSTs vs. phrase-based SFSTs is compared
  in (González and Casacuberta, 2007).

## Troubleshooting
- Building the iGREAT package

If for some reason, the building process does not work, try:
```
	$ ./bootstrap.sh
```

just before the sequence `./configure; make; make install` is invoked.

## Literature
```
@ARTICLE{,
 author  = {Jorge González and Francisco Casacuberta},
 title   = {GREAT: open source software for statistical machine translation},
 journal = {Machine translation},
 year    = {2011},
 volume  = {25},
 number  = {2},
 pages   = {145-160},
}

@PHDTHESIS{,
 author  = {Jorge González},
 title   = {Aprendizaje de Transductores Estocásticos de Estados Finitos
            y su Aplicación en Traducción Automática},
 school  = {Universidad Politécnica de Valencia},
 year    = {2009},
 address = {Valencia (Spain)},
 month   = {July},
 note    = {Advisor: F. Casacuberta},
}

@INPROCEEDINGS{,
     author = {González, Jorge and Casacuberta, Francisco},
      month = {March 30},
      title = {{GREAT: a finite-state machine translation toolkit
                implementing a Grammatical Inference Approach for
                Transducer Inference (GIATI)}},
  booktitle = {{EACL Workshop on Computational Linguistics Aspects of
                Grammatical Inference}},
       year = {2009},
      pages = {24--32},
    address = {Athens, Greece}
}

@INPROCEEDINGS{,
     author = {González, Jorge and Sanchis-Trilles, Germán and
               Casacuberta, Francisco},
      month = {February 17 to 23},
      title = {Learning finite state transducers using bilingual
               phrases},
  booktitle = {Proceedings of the 9th International Conference on
               Intelligent Text Processing and Computational
               Linguistics. Lecture Notes in Computer Science},
     volume = {4919},
       year = {2008},
      pages = {411-422},
    address = {Haifa, Israel}
}

@INPROCEEDINGS{,
     author = {González, Jorge and Casacuberta, Francisco},
      month = {February 14},
      title = {{Linguistic Categorisation in Machine Translation
                using Stochastic Finite State Transducers}},
  booktitle = {Mixing Approaches to Machine Translation},
       year = {2008},
    address = {San Sebastian, Spain},
}

@INPROCEEDINGS{,
     author = {González, Jorge and Casacuberta, Francisco},
      month = {September 14-16},
      title = {Phrase-based finite state models},
  booktitle = {Proceedings of the 6th International Workshop on
               Finite State Methods and Natural Language
               Processing (FSMNLP)},
       year = {2007},
    address = {Potsdam (Germany)},
}
```

## History
* Markdown formatting: *10 Mar. 2021*, **Miguel Domingo**.
* Updated:  *3 Nov. 2011*, **Jorge González**.
* Edited:  *24 Feb. 2011*, **Jorge González**.
