# learnscript-ai (WIP)
I'm probably going to completely change how this works in the future. As I think more about it. Also recomend ideas to me.  
### Explanation:  
The scripting language is console driven, when you start the console for the first time it will add a registry entry to open files with the .lnscr, .lndb and .lnscore. You talk to the console like a shell and execute programs, some programs are questions, some are entire study session managers, and some are built-in. There are also global variables to manage your session.  

There is a database which is a nested list of identifiers called categories and a table for the sizes of each category so you can skip them easily when scrubbing the file. Each category has a value and a list of tags, and then children. A category like HTML may have children "attributes" and "elements", and the elements category has children of each html element which are tagged with the name the attributes it can have. The attributes child has children of each html attribute. You could create a question to list all the childen of a category, or specific children of a category, or the value of a single or multiple categories. You can handle category names however you like. Basically json I guess? I think knowledge is fundementally parent/child + data. In the human brain. The questions which are natural are, What did it come from? (parent) What can it be made into or what does it do? (child) What differentiates it from things in the same category? (data). If you know these three things it feels like you "know" or "understand" something.

An .lnscr and .lndb go together, the .lnscr (learn script) is a list of functions (questions) which share identifiers with categories in the .lndb file (learn database). And the .lnscore stores the specific information for a profile of a certain user and database. It has the score for each category and subcategory in .lndb and the neural net profile, you can import these neural nets from another database.

The database will also specify several question formats. These formats specify inputs to the neural network whose parameters will be determined only once for a profile\database pair. Adding new questions it's best to stick to as few formats as possible unless you think they are psycologically diverse. (don't worry about this part, all ai stuff will be nearly automatic). Score as well as other parameters will be processed through a neural net (transformed) then the questions with the lowest scores will be recommended. There will be pretrained networks that will only tweak a few meta-parameters if you follow certain question formats. For example someone with a larger working memory will increase the slope of some curve. Questions with a score of 0 are ignored.

Each category in .lndb can have certain types of tags:  
norec means do not recomend this question  
noscore means do not score this queston, (score will be calculated through a neural net of the sub scores)  
Any of the other categories' identifiers to denote it as a child of that category but this is not recursive
Any name that isn't a category which is a just a tag. 
I'm thinking categories can be re-declared?

A category that has no function with its same name in .lnscr it can still be used like other categories but it will behave differently.  

The console provides a built-in default script which will just call the most recomended questions after prompting for a .lnscore to use.  
All scripting functions are available as console commands when no script is running or if you prepend a tilde to what you type.  

### Some example functions:  
`recQuestion(int num, char **arr);` Recomend a question from the current directory or specified tags.  
`setCategory();` Set the category of questions  
`askQuestion();` Asks a question, if there is no associated question it will ask the highest recomended question.  
`addQuestion();`  

You will be able to print text, clear the console, set the console into different modes for pretty printing, define new questions with a simple text editor, keep a question history log, use a provided algorithm that looks for keywords and meta-grammer to accept answers that are similair, get a list of child categories and the parent categories. The history will be accessable globally.
