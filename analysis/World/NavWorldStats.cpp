//
//  NavWorldStats.cpp
//  MarkovBrain.4.0
//
//  Created by Leigh Sheneman on 1/27/16.
//  Copyright © 2016 Leigh Sheneman. All rights reserved.
//

#include "NavWorldStats.h"

int& NavWorldStats::stepsToGoal = Parameters::register_parameter("NAV_stepsToGoal", xDim/2, "minimum steps to goal", "WORLD - NAV");


Pos Pos::newPos(int X,int Y){
    Pos P;
    P.x=X;
    P.y=Y;
    return P;
}

NavWorldStats::NavWorldStats(){
    area.resize(xDim);
    distMap.resize(xDim);
    dirMap.resize(xDim);
    for(int i=0;i<xDim;i++){
        area[i].resize(yDim);
        distMap[i].resize(yDim);
        dirMap[i].resize(yDim);
    }
}

int NavWorldStats::makeNumberNotBorder(int range){
    int n;
    do{
        n=n=Random::getInt(range);
    }while((n==0)||(n==range-1));
    return n;
}

void NavWorldStats::makeMap(){
    vector<Pos> current,next;
    int cDist=1;
//    int i,j,k;
    do{
        cDist=1;
        for(int i=0;i<xDim;i++)
            for(int j=0;j<yDim;j++){
                distMap[i][j]=-1;
                area[i][j]=-1;
                dirMap[i][j]=-1;
                if((i==0)||(i==(xDim-1))||(j==0)||(j==(yDim-1))||((rand()&7)==0))
                    area[i][j]=0;
            }
        
        targetX=makeNumberNotBorder(xDim);
        targetY=makeNumberNotBorder(yDim);
        distMap[targetX][targetY]=0;
        cout<<targetX<<" : "<<targetY<< endl;
        current.clear();
        next.clear();
        current.push_back(Pos::newPos(targetX,targetY));
        while(current.size()!=0){
            for(int i=0;i<current.size();i++){
                for(int j=0;j<4;j++){
                    if((area[current[i].x+xm4[j]][current[i].y+ym4[j]]!=0)
                       &&(distMap[current[i].x+xm4[j]][current[i].y+ym4[j]]==-1)){
                        distMap[current[i].x+xm4[j]][current[i].y+ym4[j]]=cDist;
                        next.push_back(Pos::newPos(current[i].x+xm4[j],current[i].y+ym4[j]));
                    }
                }
            }
            current.clear();
            current=next;
            next.clear();
            cDist++;
        }
    }while(cDist<xDim/2);
    do{
        startX=rand()&(xDim-1);
        startY=rand()&(yDim-1);
    } while(distMap[startX][startY]!=stepsToGoal);
    int maxSteps=0;
    for(int i=1;i<xDim-1;i++)
        for(int j=1;j<yDim-1;j++)
            if(distMap[i][j]>0){
                int mD=distMap[i][j];
                dirMap[i][j]=0;
                for(int k=0;k<4;k++)
                    if(distMap[i+xm4[k]][j+ym4[k]]!=-1){
                        if(distMap[i+xm4[k]][j+ym4[k]]<mD){
                            mD=distMap[i+xm4[k]][j+ym4[k]];
                            dirMap[i][j]=k;
                        }
                    }
                if(maxSteps<distMap[i][j]){
                    maxSteps=distMap[i][j];
                }
            }
    cout<<"Max steps: "<<maxSteps<< endl;
    dirMap[targetX][targetY]=rand()&3;;
}

void NavWorldStats::evaluateFitness(vector<shared_ptr<Organism>> population,bool analyse){

    makeMap();
    // cout<<endl<<"Distance Map:"<<endl<<endl;
    // for(int i=1;i<xDim-1;i++){
    //     for(int j=1;j<yDim-2;j++){
    //         cout<<distMap[i][j]<<",";
    //     }
    //     cout<<distMap[i][yDim-2]<<endl;
    // }
    
    // cout<<endl<<"Direction Map:"<<endl<<endl;
    // for(int i=1;i<xDim-1;i++){
    //     for(int j=1;j<yDim-2;j++){
    //         cout<<dirMap[i][j]<<",";
    //     }
    //     cout<<dirMap[i][yDim-2]<<endl;
    // }
   for (size_t i = 0; i < population.size(); i++) {
       for (int m = 0; m < 24; m++) {
           currentMapID=m;
          FeedbackGate::feedbackON=true;
          cout<<"Map "<< currentMapID<<" Feedback: "<< FeedbackGate::feedbackON<<endl;
          testIndividual(population[i], analyse);
           FeedbackGate::feedbackON=false;
           cout<<"Map "<< currentMapID<<" Feedback: "<< FeedbackGate::feedbackON<<endl;
           testIndividual(population[i], analyse);
           
       }
       population[i]->dataMap.Set("update", Global::update);
   }
}

double NavWorldStats::testIndividual(shared_ptr<Organism> org, bool analyse){
    int xPos,yPos,dir;
    int t;
    int goalsReached=0;
    string timeDelta="";
    int steps=4*(xDim+yDim);
    int i;
    org->dataMap.Append("target", to_string(targetX)+":"+to_string(targetY));

    
    if( FeedbackGate::feedbackON ){
        org->dataMap.Append("goalTimesFBON"+to_string(currentMapID), 0);
        cout<<"On: "<< org->computeMutualInfo()<<endl;
        org->dataMap.Set("mutualInfoBirth"+to_string(currentMapID), org->computeMutualInfo());
    }
    else{
        org->dataMap.Append("goalTimesFBOFF"+to_string(currentMapID), 0);
    }
    
    org->brain->resetBrain();
    xPos=startX;
    yPos=startY;
    dir=Random::rand()&3;
    double fitness=0.0;
    int A[4]={0,0,0,0};
    for(t=0;t<steps;t++){
        if( FeedbackGate::feedbackON )
            org->dataMap.Append("postionsFBON_x:y"+to_string(currentMapID), to_string(xPos) +":"+to_string(yPos));
        for(i=0;i<4;i++)
            org->brain->states[i]=0.0;
        org->brain->states[(dirMap[xPos][yPos]-dir)&3]=1.0;
        org->brain->update();
        int action=(Bit(org->brain->states[org->brain->nrOfBrainStates-2])<<1)+Bit(org->brain->states[org->brain->nrOfBrainStates-1]);
        A[mapping[currentMapID][action]]++; //-> data map
        switch(mapping[currentMapID][action]){
            case 0:break;
            case 1:dir=(dir-1)&3;break;
            case 2:dir=(dir+1)&3;break;
            case 3:if(distMap[xPos+xm4[dir]][yPos+ym4[dir]]!=-1){
                xPos=xPos+xm4[dir];
                yPos=yPos+ym4[dir];
            }
                break;
        }
        fitness+=1.0/pow((double)(distMap[xPos][yPos]+1),2.0);
        
        if (distMap[xPos][yPos] == 0)
        {
            fitness+=1000.0;
            
            goalsReached++;
            do{
                xPos=rand()%xDim;
                yPos=rand()%yDim;
            }while (distMap[xPos][yPos] != stepsToGoal);
            dir=Random::getIndex(3);
            org->computeMutualInfo();
//            if( FeedbackGate::feedbackON )
//                org->dataMap.Append("goalTimesFBON"+to_string(currentMapID), t);
//            else
//                org->dataMap.Append("goalTimesFBOFF"+to_string(currentMapID), t);
        }
    }
    cout<<goalsReached<<endl;

    if( FeedbackGate::feedbackON ){
        org->dataMap.Set("goalReachedFBON"+to_string(currentMapID), goalsReached);
        org->dataMap.Set("gateRSM_FBON"+to_string(currentMapID), org->computeRMS());
        org->dataMap.Set("mutualInfoDeath"+to_string(currentMapID), org->computeMutualInfo());
        org->dataMap.Set("PositiveFB"+to_string(currentMapID),org->getAppliedPosFeedback());
        org->dataMap.Set("NegativeFB"+to_string(currentMapID),org->getAppliedNegFeedback());
        //  cout<<"amm ount"<<org->getAppliedPosFeedback()+org->getAppliedNegFeedback()<<endl;
        org->dataMap.Set("channelEntropy"+to_string(currentMapID),org->computeChannelEntropy());
        org->dataMap.Set("nopFBON",A[0]);
        org->dataMap.Set("turnFBON",A[1]+A[2]);
        org->dataMap.Set("goFBON",A[3]);
    }
    else{
        org->dataMap.Set("channelEntropyFBOFF"+to_string(currentMapID),org->computeChannelEntropy());
        org->dataMap.Set("goalReachedFBOFF"+to_string(currentMapID), goalsReached);
        org->dataMap.Set("nopFBOFF",A[0]);
        org->dataMap.Set("turnFBOFF",A[1]+A[2]);
        org->dataMap.Set("goFBOFF",A[3]);
    }

    if(fitness<0.0)
        fitness=0.0;
    return fitness;
}
