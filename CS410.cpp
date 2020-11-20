// basic file operations

#include "CS410.h"


/*
    Constructor
 */
CorpusClass::CorpusClass() {
}

/*
    Destructor
 */
CorpusClass::~CorpusClass() {
}

/*
    Construct a list of unique words in the whole corpus. Put it in self.vocabulary
    for example: ["rain", "the", ...]
 */
void CorpusClass::build_vocabulary()
{
   fstream newfile;
   newfile.open(document_path,ios::in); //open a file to perform read operation using file object
   if (newfile.is_open()) //checking whether the file is open
   {   
        string tp;
        while(getline(newfile, tp)) //read data from file object and put it into string.
        {
            //cout << tp << "\n"; //print the data of the string
            map <string, int> docwords;
            istringstream iss(tp);
            vector<string> results(istream_iterator<string>{iss}, istream_iterator<string>());      
            for (vector<string>::iterator wordit = results.begin() ; wordit != results.end(); ++wordit)
            {
                if(*wordit != "0" && *wordit != "1")        // $$$ CAN REMOVE THIS WHEN WE GET DATA
                {
                    //std::cout << ' ' << *it; 
                    // update vocabulary map
                    map <string, int>::iterator  mapit;
                     mapit = VocabularyMap.find(*wordit);
                    if (mapit != VocabularyMap.end())
                    {
                        mapit->second++;
                    }
                    else
                    {
                        VocabularyMap.insert(make_pair(*wordit, 1));
                    }
                }
            }
        }

        VocublarySize = VocabularyMap.size();
        cout << "Vocublary Size: " << VocublarySize << "\n";
        newfile.clear();
        newfile.seekg(0, ios::beg);
        
        DocumentNumber = 0;
        while(getline(newfile, tp)) //read data from file object and put it into string.
        {
            //cout << tp << "\n"; //print the data of the string
            //cout << "oy \n";
            map <string, int> docwords;
            istringstream iss(tp);
            vector<string> results(istream_iterator<string>{iss}, istream_iterator<string>());      
            map <string, int>::iterator  mapit;
            for (vector<string>::iterator wordit = results.begin() ; wordit != results.end(); ++wordit)
            {
                if(*wordit != "0" && *wordit != "1")        // $$$ CAN REMOVE THIS WHEN WE GET DATA
                {
                    mapit = docwords.find(*wordit);
                    if (mapit != docwords.end())
                    {
                        mapit->second++;
                        //cout << "OY " << mapit->second << "\n";
                    }
                    else
                    {
                        docwords.insert(make_pair(*wordit, 1));
                    }
                }
            }
            //cout << "Words in this doc: " << docwords.size() << "\n";
            DocumentNumber++;
            //DocumentWords.push_back(docwords);
            vector<int> wordcount;
            // for each word
            for (map <string, int>::iterator wordit = VocabularyMap.begin() ; wordit != VocabularyMap.end(); ++wordit)
            {
                map <string, int>::iterator  mapit;
                int vocabcount = 0;
                mapit = docwords.find(wordit->first);
                if (mapit != docwords.end())
                {
                    vocabcount = mapit->second;
                }
                wordcount.push_back(vocabcount);
                //cout << "word Count: " << vocabcount << "\n";
            }
            //cout << "words: " << wordcount.size() << "\n";
            DocumentWordMatrix.push_back(wordcount);
         }

        newfile.close(); //close the file object.

        cout << "Documents: " << DocumentNumber << "\n";
        cout << "Doc Word matrix complete: " << "\n";
   }   
}
void CorpusClass::initialize_prob()
{
    srand(static_cast<unsigned int>(std::time(nullptr))); 
    //vector<vector<double>> DocTopicProb;
    // doc topic prob
    for(int i = 0; i < DocumentNumber; i++)
    {
        vector<double> prob;
        double probsum = 0;
        int yy;
        for(yy = 0; yy < number_of_topics; yy++)
        {
            //double newprob = 1;
            double newprob = (double)rand()/RAND_MAX; 
            probsum += newprob;
            prob.push_back(newprob);
        }
        for(yy = 0; yy < number_of_topics; yy++)
        {
            prob[yy] = prob[yy]/probsum;
        }
        DocTopicProb.push_back(prob);
    }
    
    //vector<vector<double>> TopicWordProb;
    for(int ii = 0; ii < number_of_topics; ii++)
    {
        vector<double> prob;
        double probsum = 0;
        int yy;
        for(yy = 0; yy < VocublarySize; yy++)
        {
            double newprob = (double)rand()/RAND_MAX; 
            //double newprob = 1;
            probsum += newprob;
            prob.push_back(newprob);
        }
        for(yy = 0; yy < VocublarySize; yy++)
        {
            prob[yy] = prob[yy]/probsum;
        }
        TopicWordProb.push_back(prob);
    }
    //cout << "Initialization complete: " << "\n";

}

void CorpusClass::setup_array()
{
    pdocarray = new double**[DocumentNumber];
    for (int i = 0; i < DocumentNumber; i++)
    {
        double** ptopicarray = new double*[number_of_topics];
        //cout << "Allocated topic array \n";
        pdocarray[i] = ptopicarray;
        for (int ii = 0; ii < number_of_topics; ii++)
        {
            double* pwordarray = new double[VocublarySize];
            //cout << "Allocated word array \n";
            ptopicarray[ii] = pwordarray;
            //for (int iii = 0; iii < VocublarySize; iii++)
                //pwordarray[iii] = 0;
        }
    }
    cout << "E Initialization Setup \n";
}

void CorpusClass::delete_array()
{
    for (int i = 0; i < DocumentNumber; i++)
    {
        double** ptopicarray = pdocarray[i];
        for (int ii = 0; ii < number_of_topics; ii++)
        {
            double* pwordarray = ptopicarray[ii];
            delete[] pwordarray;
        }
        delete[] ptopicarray;
    }
    delete[] pdocarray;
}

void CorpusClass::run_iteration()
{
    //cout << "E Step \n";
    //cout << "Allocated doc array \n";
    // IS THIS EVEN NECESSARY?
    /*
    for (int i = 0; i < DocumentNumber; i++)
    {
        double** ptopicarray = pdocarray[i];
        for (int ii = 0; ii < number_of_topics; ii++)
        {F
            double* pwordarray = ptopicarray[ii];
            for (int iii = 0; iii < VocublarySize; iii++)
                pwordarray[iii] = 1;
        }
    }
    cout << "E Initialization Complete \n";
    */
    for(int docindex = 0; docindex < DocumentNumber; docindex++)
    {
        //cout << "E Doc \n";
        for(int wordindex = 0; wordindex < VocublarySize; wordindex++)
        {
            double mysum = 0;
            //cout << "E Word \n";
            for(int topicindex = 0; topicindex < number_of_topics; topicindex++)
            {
                //cout << "E Topic \n";
                double newval = TopicWordProb[topicindex][wordindex] * DocTopicProb[docindex][topicindex];
                //cout << "E Topic1 \n";
                double** ptopicarray = pdocarray[docindex];
                double* pwordarray = ptopicarray[topicindex];
                pwordarray[wordindex] = newval;

                mysum += newval;
            }
            for(int topicindex = 0; topicindex < number_of_topics; topicindex++)
            {
                //cout << "E Topic2 \n";
                double** ptopicarray = pdocarray[docindex];
                double* pwordarray = ptopicarray[topicindex];
                pwordarray[wordindex] = pwordarray[wordindex]/mysum;
            }
        }
   }
    
    //cout << "M Step \n";
    for(int docindex = 0; docindex < DocumentNumber; docindex++)
    {
        double tonormal = 0;
        for(int topicindex = 0; topicindex < number_of_topics; topicindex++)
        {
            double mysum = 0;
            for(int wordindex = 0; wordindex < VocublarySize; wordindex++)
            {
                double** ptopicarray = pdocarray[docindex];
                double* pwordarray = ptopicarray[topicindex];
                double newval = pwordarray[wordindex] * DocumentWordMatrix[docindex][wordindex];
                mysum += newval;
                //cout << "probs: " << DocumentWordMatrix[docindex][wordindex] << "\n";
            }
            DocTopicProb[docindex][topicindex] = mysum;
            //cout << "DocTopicProb: " << DocTopicProb[docindex][topicindex] << "\n";
            tonormal += mysum;
        }
        // normalize
        for(int topicindex = 0; topicindex < number_of_topics; topicindex++)
        {
            DocTopicProb[docindex][topicindex] = DocTopicProb[docindex][topicindex]/tonormal;
            //cout << " " << DocTopicProb[docindex][topicindex] << " ";
        }
        //cout << "\n";
    }
    for(int topicindex = 0; topicindex < number_of_topics; topicindex++)
    {
        double tonormal = 0;
        for(int wordindex = 0; wordindex < VocublarySize; wordindex++)
        {
            double mysum = 0;
            for(int docindex = 0; docindex < DocumentNumber; docindex++)
            {
                double** ptopicarray = pdocarray[docindex];
                double* pwordarray = ptopicarray[topicindex];
                double newval = pwordarray[wordindex] * DocumentWordMatrix[docindex][wordindex];
                mysum += newval;
            }
            TopicWordProb[topicindex][wordindex] = mysum;
            tonormal += mysum;
        }
        for(int wordindex = 0; wordindex < VocublarySize; wordindex++)
        {
            TopicWordProb[topicindex][wordindex] = TopicWordProb[topicindex][wordindex]/tonormal;
            //cout << " " << TopicWordProb[topicindex][wordindex] << " ";
        }
        //cout << "\n";
    }
}

double CorpusClass::calculate_likelyhood()
{
    double newlikely = 0;
    for(int docindex = 0; docindex < DocumentNumber; docindex++)
    {
        long double docsum = 0;
        for(int wordindex = 0; wordindex < VocublarySize; wordindex++)
        {
            long double mysum = 0;
            //cout << "DocTopicProb: " ;
            for(int topicindex = 0; topicindex < number_of_topics; topicindex++)
            {
                //cout <<  TopicWordProb[topicindex][wordindex]  << " ";
                //cout <<  DocTopicProb[docindex][topicindex] << " ";
                //cout <<  TopicWordProb[topicindex][wordindex] * DocTopicProb[docindex][topicindex] << ", ";
                mysum += TopicWordProb[topicindex][wordindex] * DocTopicProb[docindex][topicindex];
            }
            //cout << "mysum: " << mysum << "\n";
            //cout << "\n";
            //cout << " " << mysum << ",";
            mysum = log10(mysum);
            //cout << mysum << ",";
            //cout << DocumentWordMatrix[docindex][wordindex] << ",";
            //cout << mysum << ",";
            mysum = mysum * DocumentWordMatrix[docindex][wordindex];
            //cout << mysum << " ";
            docsum += mysum;
        }
        //cout << "\n";
        newlikely += docsum;
        //cout << "docsum: " << docsum << "\n";
        
        //docindex = DocumentNumber;
    }
    return newlikely;
}

void CorpusClass::PLSA()
{

    double current_likelihood = calculate_likelyhood();
    cout << "Baseline Likely: " << current_likelihood << "\n";
    for (int i = 0; i < max_iterations; i++)
    {
        cout << "Iteration: " << i << "\n";
        try 
        {
            run_iteration();         
        }
        catch (const std::bad_alloc& e) 
        {
            std::cout << "Allocation failed: " << e.what() << '\n';
            return;
        }        
        double newlikely = calculate_likelyhood();
        cout << "Likely: " << newlikely << "\n";
        if(abs(current_likelihood - newlikely) < epsilon)
        {
            cout << "Converge \n";
            return;       
        }
        current_likelihood = newlikely;
    }
}

int main () {
    CorpusClass corpus;
    //corpus.document_path = "data/DBLP2.txt";
    corpus.document_path = "data/test.txt";
    corpus.number_of_topics = 2;
    corpus.max_iterations = 500;
    corpus.epsilon = 0.001;
    
    corpus.build_vocabulary();
    cout << "build_vocabulary \n";
    corpus.initialize_prob();
    cout << "initialize_prob \n";
    corpus.setup_array();
    cout << "setup_array \n";
  
    corpus.PLSA();
    
    corpus.delete_array();


    return 0;
}