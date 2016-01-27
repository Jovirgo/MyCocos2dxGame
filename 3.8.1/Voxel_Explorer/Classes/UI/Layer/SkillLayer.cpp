//
//  Skilllayer.cpp
//  lichuang
//
//  Created by lichuang on 2016/01/13.
//
//

#include "SkillLayer.hpp"
#include "UtilityHelper.h"
#include "PlayerProperty.hpp"
#include "VoxelExplorer.h"
#include "Player.hpp"
#include "PopupUILayerManager.h"
#include "PotionsProperty.hpp"
USING_NS_CC;
SkillLayer::SkillLayer()
{
    m_pSkill    = nullptr;
    m_pMagicPotion = nullptr;
    m_pBloodPotion = nullptr;
    m_pMagicProgress = nullptr;
    m_pBloodProgress = nullptr;
    m_pSkillProgress = nullptr;
    m_bIsUseSkill   = false;
    m_nMagicPotionId   = -1;
    m_nBloodPotionId = -1;
    m_fCoolTime = 0.0f;
    m_bIsRloadEquip = false;
    m_pMagicPotionMask = nullptr;
    m_pBloodPotionMask = nullptr;
    m_bIsUseBloodPotion = false;
    m_bIsUseMagicPotion = false;
}

SkillLayer::~SkillLayer()
{
    
}

bool SkillLayer::initUi()
{
    if (!WrapperUILayer::initUi())
            return false;
    return load("skillLayer.csb",false);
}
bool SkillLayer::addEvents()
{

    m_pSkill = dynamic_cast<cocos2d::ui::ImageView *>(UtilityHelper::seekNodeByName(m_pRootNode, "skill_icon"));
    if(!m_pSkill)
        return false;
    m_pMagicPotion = dynamic_cast<cocos2d::ui::ImageView *>(UtilityHelper::seekNodeByName(m_pRootNode, "skill_magic_icon"));
    if(!m_pMagicPotion)
        return false;
    m_pBloodPotion = dynamic_cast<cocos2d::ui::ImageView *>(UtilityHelper::seekNodeByName(m_pRootNode, "skill_health_icon"));
    if(!m_pBloodPotion)
        return false;
    
    m_pMagicPotionMask = dynamic_cast<cocos2d::ui::ImageView *>(UtilityHelper::seekNodeByName(m_pRootNode, "skill_magic_icon_mask"));
    if(!m_pMagicPotionMask)
        return false;
    m_pBloodPotionMask = dynamic_cast<cocos2d::ui::ImageView *>(UtilityHelper::seekNodeByName(m_pRootNode, "skill_health_icon_mask"));
    if(!m_pBloodPotionMask)
        return false;
    m_pMagicPotionMask->setVisible(false);
    m_pBloodPotionMask->setVisible(false);
    
    m_pSkill->addTouchEventListener(CC_CALLBACK_2(SkillLayer::onTouchSkill, this));
    m_pMagicPotion->addTouchEventListener(CC_CALLBACK_2(SkillLayer::onTouchMagic, this));
    m_pBloodPotion->addTouchEventListener(CC_CALLBACK_2(SkillLayer::onTouchBlood, this));
    
    m_pSkillProgress = ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("ui_skill_prog.png"));
    m_pSkillProgress->setType(cocos2d::ProgressTimer::Type::RADIAL);
    m_pSkillProgress->setReverseDirection(true);
    m_pSkillProgress->setPosition(m_pSkill->getContentSize()*0.5);
    m_pSkill->addChild(m_pSkillProgress);
    
    m_pBloodProgress = ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("ui_skill_prog.png"));
    m_pBloodProgress->setType(cocos2d::ProgressTimer::Type::RADIAL);
    m_pBloodProgress->setReverseDirection(true);
    m_pBloodProgress->setPosition(m_pBloodPotion->getContentSize()*0.5);
    m_pBloodPotion->addChild(m_pBloodProgress);
    m_pBloodProgress->setPercentage(0);
    
    m_pMagicProgress = ProgressTimer::create(cocos2d::Sprite::createWithSpriteFrameName("ui_skill_prog.png"));
    m_pMagicProgress->setType(cocos2d::ProgressTimer::Type::RADIAL);
    m_pMagicProgress->setReverseDirection(true);
    m_pMagicProgress->setPosition(m_pMagicPotion->getContentSize()*0.5);
    m_pMagicPotion->addChild(m_pMagicProgress);
    m_pMagicProgress->setPercentage(0);
    
    
    m_pMagicPotion->setTouchEnabled(true);
    m_pSkill->setTouchEnabled(true);
    m_pBloodPotion->setTouchEnabled(true);
    refreshUIView();
    refreshSkillView(true);

    
    return true;
}
void SkillLayer::onEnter()
{
    WrapperUILayer::onEnter();
    EventListenerCustom *listener = EventListenerCustom::create(EVENT_PLAYER_USE_POTION, CC_CALLBACK_1(SkillLayer::onEventRoleUserPotion,this));
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener,this);
    
}
void SkillLayer::onExit()
{
    Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
    WrapperUILayer::onExit();
}
void SkillLayer::refreshUIView()
{
    std::string imgName = "ui_skill_lock.png" ;
    if (m_pMagicPotion && m_pBloodPotion)
    {
        m_nMagicPotionId = -1;
        m_nBloodPotionId = -1;
        std::vector<PickableItemProperty*> items = PlayerProperty::getInstance()->getPlayerBag();
        std::sort(items.begin(), items.end(), [](PickableItemProperty* item1,PickableItemProperty* item2){
                return item1->getPickableItemType() < item2->getPickableItemType();
        });
        for (auto iter =items.begin(); iter!=items.end(); ++iter)
        {
            if (*iter) {
                PickableItemProperty* item = (*iter);
                if (item->getPickableItemType()>=PickableItem::PIT_POTION_MINORHEALTH && item->getPickableItemType()<=PickableItem::PIT_POTION_HEALTH)
                {
                    m_nBloodPotionId = item->getInstanceID();
                    break;
                }
            }
        }
        for (auto iter =items.begin(); iter!=items.end(); ++iter)
        {
            if (*iter) {
                PickableItemProperty* item = (*iter);
                if (item->getPickableItemType()>=PickableItem::PIT_POTION_MINORMANA && item->getPickableItemType()<=PickableItem::PIT_POTION_MANA)
                {
                    m_nMagicPotionId = item->getInstanceID();
                    break;
                }
            }
        }
        if(!m_bIsUseMagicPotion)
        {
            if (m_nMagicPotionId ==-1){
                m_pMagicProgress->setPercentage(100);
            }else
                m_pMagicProgress->setPercentage(0);
        }

        if(!m_bIsUseBloodPotion)
        {
            if (m_nBloodPotionId ==-1){
                m_pBloodProgress->setPercentage(100);
            }else
                m_pBloodProgress->setPercentage(0);
  
        }
        
    }
    
}
void SkillLayer::refreshSkillView(bool isRloadEquip)
{
    m_bIsRloadEquip = isRloadEquip;
    std::string imgName = "ui_skill_lock.png" ;
    if (m_pSkill) {
        m_pSkillProgress->setPercentage(100);
        m_bIsUseSkill = false;
        CChaosNumber secondWeaponId =  PlayerProperty::getInstance()->getEquipedSecondWeaponID();
        
        if ( secondWeaponId.GetLongValue() != -1)
        {
            
            PickableItemProperty* item = PlayerProperty::getInstance()->getItemFromBag( secondWeaponId);
            if (item) {
                PickableItem::PickableItemType type = item->getPickableItemType();
                if (type >= PickableItem::PIT_BOW_SHORTBOW && type <= PickableItem::PIT_BOW_PRO_GOLDENBOW)//魔法箭
                {
                    imgName = "ui_skill_arrow.png";
                    m_bIsUseSkill = true;
                    m_fCoolTime = Player::m_fMagicArrowColdDownTime;

                }
                else if(type >= PickableItem::PIT_STAFF_OAKSTAFF && type <= PickableItem::PIT_STAFF_PRO_MONKSTAFF)//火球
                {
                    imgName = "ui_skill_fire.png";
                    m_bIsUseSkill = true;
                    m_fCoolTime = Player::m_fFireBallColdDownTime;

                }
                else if(type >= PickableItem::PIT_SHIELD_WOODENSHIELD && type <= PickableItem::PIT_SHIELD_PRO_TOWERSHIELD)//格挡
                {
                    imgName = "ui_skill_block.png";
                    m_bIsUseSkill = true;
                    m_fCoolTime = Player::m_fBlockRateUpColdDownTime;

                }
                else
                    imgName = "ui_skill_lock.png";
            }
            
        }
        if (m_bIsRloadEquip) {
            m_bIsRloadEquip = false;
            m_fCoolTime = 5.0f;
        }
        if (m_bIsUseSkill){
                skillProgressAction();
        }else
            m_pSkillProgress->setPercentage(0);
        
        
        m_pSkill->loadTexture(imgName,TextureResType::PLIST);
        m_pSkillProgress->setCameraMask((unsigned short)m_nCamerFlag);
        m_pSkill->setCameraMask((unsigned short)m_nCamerFlag);
    }

}
void SkillLayer::onEventRoleUserPotion(cocos2d::EventCustom *sender)
{

    PotionsProperty* potionsProperty = static_cast<PotionsProperty*>(sender->getUserData());
    PickableItem::PickableItemType itemType = potionsProperty->getPickableItemType();
    if (itemType>=PickableItem::PIT_POTION_MINORHEALTH && itemType<=PickableItem::PIT_POTION_HEALTH) {
        if (m_nBloodPotionId!=-1) {
            m_pBloodProgress->stopAllActions();
            m_pBloodProgress->setPercentage(100);
            bloodProgressAction();
            m_bIsUseBloodPotion = true;
        }

    }
    if (itemType>=PickableItem::PIT_POTION_MINORMANA && itemType<=PickableItem::PIT_POTION_MANA) {
        if (m_nMagicPotionId!=-1)
        {
            m_pMagicProgress->stopAllActions();
            m_pMagicProgress->setPercentage(100);
            magicProgressAction();
            m_bIsUseMagicPotion = true;
        }
       
    }
}
void SkillLayer::onTouchBlood(Ref* ref,Widget::TouchEventType type)
{
       switch (type) {
        case Widget::TouchEventType::BEGAN:
              
            if (m_pBloodPotion) {
                m_pBloodPotion->setScale(0.3);
            }
            if (m_nBloodPotionId==-1)
                clickEffect();
               
            break;
        case Widget::TouchEventType::ENDED:
               this->stopAllActions();
               m_pBloodPotion->runAction(Sequence::create(EaseBackInOut::create(ScaleTo::create(0.1f, 0.35f)), CallFunc::create([this](){
                   if (m_pBloodPotion) {
                       m_pBloodPotion->stopAllActions();
                       
                       if (m_nBloodPotionId!=-1)
                       {
                           if (m_pBloodProgress->getPercentage()==0)
                           {
                               CCLOG("USE Blood");
                               PlayerProperty::getInstance()->usePotion(m_nBloodPotionId);
                           }else
                           {
                               CCLOG("SKILL_TIP_POTION_COOLING");
                               PopupUILayerManager::getInstance()->showStatusImport(TIP_NEGATIVE, UtilityHelper::getLocalStringForUi("SKILL_TIP_POTION_COOLING"));
                           }
                           
                           
                       }else
                       {
                           CCLOG("SKILL_TIP_NOT_BLOOD_POTION");
                           PopupUILayerManager::getInstance()->showStatusImport(TIP_NEGATIVE, UtilityHelper::getLocalStringForUi("SKILL_TIP_NOT_BLOOD_POTION"));
                       }
                       
                   }
                   
               }),nullptr));
               
            break;
        case Widget::TouchEventType::MOVED:
        case Widget::TouchEventType::CANCELED:
           if (m_pBloodPotion) {
               m_pBloodPotion->stopAllActions();
               m_pBloodPotion->runAction(EaseBackInOut::create(ScaleTo::create(0.2f, 0.35f)));
           }
               break;
            break;
        default:
            break;
    }
}
void SkillLayer::onTouchMagic(Ref* ref,Widget::TouchEventType type)
{
    switch (type) {
        case Widget::TouchEventType::BEGAN:
            
            if (m_pMagicPotion) {
                m_pMagicPotion->setScale(0.3);
            }
            if (m_nMagicPotionId==-1)
                clickEffect();
            break;
        case Widget::TouchEventType::ENDED:
            this->stopAllActions();
            m_pMagicPotion->runAction((Sequence::create(EaseBackInOut::create(ScaleTo::create(0.1f, 0.35f)),CallFunc::create([this](){
                
                if (m_pMagicPotion) {
                    m_pMagicPotion->stopAllActions();
                    
                    if (m_nMagicPotionId!=-1)
                    {
                        if (m_pMagicProgress->getPercentage()==0) {
                            CCLOG("USE Magic");
                            PlayerProperty::getInstance()->usePotion(m_nMagicPotionId);
                        }else
                        {
                             CCLOG("SKILL_TIP_POTION_COOLING");
                            PopupUILayerManager::getInstance()->showStatusImport(TIP_NEGATIVE, UtilityHelper::getLocalStringForUi("SKILL_TIP_POTION_COOLING"));
                        }
                        
                        
                    }else
                    {
                        CCLOG("SKILL_TIP_NOT_MAGIC_POTION");
                        PopupUILayerManager::getInstance()->showStatusImport(TIP_NEGATIVE, UtilityHelper::getLocalStringForUi("SKILL_TIP_NOT_MAGIC_POTION"));
                    }
                    
                    
                }
                
                
            }),nullptr)));
          
             break;
       case Widget::TouchEventType::MOVED:
       case Widget::TouchEventType::CANCELED:
            if (m_pMagicPotion) {
                m_pMagicPotion->stopAllActions();
                m_pMagicPotion->runAction(EaseBackInOut::create(ScaleTo::create(0.2f, 0.35f)));
            }
            break;
        default:
            break;
    }
}
void SkillLayer::onTouchSkill(Ref* ref,Widget::TouchEventType type)
{
    switch (type) {
        case Widget::TouchEventType::BEGAN:
            
            if (m_pSkill) {
                m_pSkill->setScale(0.45);
            }
            if(!m_bIsUseSkill)
                clickEffect();
             break;
        case Widget::TouchEventType::ENDED:
            this->stopAllActions();
            m_pSkill->runAction(Sequence::create(EaseBackInOut::create(ScaleTo::create(0.1f, 0.5f)),CallFunc::create([this](){
                
                if (m_pSkill)
                {
                    
                    if (m_bIsUseSkill )
                    {
                        if(m_pSkillProgress->getPercentage()==0)
                        {
                            if(VoxelExplorer::getInstance()->handlePlayerUseSkill())
                                refreshSkillView();
                            
                        }else
                        {
                            PopupUILayerManager::getInstance()->showStatusImport(TIP_NEGATIVE, UtilityHelper::getLocalStringForUi("SKILL_TIP_SKILL_COOLING"));
                        }
                    }else
                    {
                        PopupUILayerManager::getInstance()->showStatusImport(TIP_NEGATIVE, UtilityHelper::getLocalStringForUi("SKILL_TIP_NOT_EQUIP"));
                    }
                    
                }
                
                
            }),nullptr));
            break;
        case Widget::TouchEventType::MOVED:
        case Widget::TouchEventType::CANCELED:
            if (m_pSkill) {
                m_pSkill->stopAllActions();
                m_pSkill->runAction(EaseBackInOut::create(ScaleTo::create(0.2f, 0.5f)));
            }
             break;
        default:
            break;
    }
}
void  SkillLayer::skillProgressAction()
{
    m_pSkillProgress->stopAllActions();
    m_pSkillProgress->runAction(Sequence::create(ProgressTo::create(m_fCoolTime, 0),CallFunc::create([this](){
        
        if (m_pSkill) {
            m_pSkillProgress->setPercentage(0);
            m_pSkill->stopAllActions();
            m_pSkill->runAction(Sequence::create(EaseBackInOut::create(ScaleTo::create(0.1f, 0.45f)), EaseBackInOut::create(ScaleTo::create(0.1f, 0.5f)),nullptr));
        }
        
    }),nullptr));
}
void SkillLayer::bloodProgressAction()
{
    if (m_pBloodProgress && m_pBloodProgress->getPercentage()==100&& m_nBloodPotionId!=-1) {
        m_pBloodProgress->stopAllActions();
        m_pBloodProgress->runAction(Sequence::create(ProgressTo::create(8.0f, 0),CallFunc::create([this](){
            
            if (m_pBloodPotion) {
                m_bIsUseBloodPotion = false;
                this->refreshUIView();
                m_pBloodPotion->stopAllActions();
                m_pBloodPotion->runAction(Sequence::create(EaseBackInOut::create(ScaleTo::create(0.1f, 0.3f)), EaseBackInOut::create(ScaleTo::create(0.1f, 0.35f)),nullptr));
            }
            
        }),nullptr));
    }
   
}
void SkillLayer::magicProgressAction()
{
    if (m_pMagicProgress && m_pMagicProgress->getPercentage()==100&&m_nMagicPotionId!=-1) {
        m_pMagicProgress->stopAllActions();
        m_pMagicProgress->runAction(Sequence::create(ProgressTo::create(8.0f, 0),CallFunc::create([this](){
            
            if (m_pMagicPotion) {
                m_bIsUseMagicPotion = false;
                this->refreshUIView();
                m_pMagicPotion->stopAllActions();
                m_pMagicPotion->runAction(Sequence::create(EaseBackInOut::create(ScaleTo::create(0.1f, 0.3f)), EaseBackInOut::create(ScaleTo::create(0.1f, 0.35f)),nullptr));
            }
            
        }),nullptr));
    }
    
}