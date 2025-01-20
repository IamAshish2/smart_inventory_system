import QuickStatCard from '@/components/custom/QuickStatCard';
import InfoCard from '@/components/custom/InfoCard';
import { db } from '@/lib/firebase/firebase';
import { onValue, ref } from 'firebase/database';
import React, { useEffect, useState } from 'react';
import { View, Text, ScrollView, StyleSheet, TouchableOpacity, SafeAreaView, Alert } from 'react-native';
import { renderDoorInfoProps } from '@/lib/types';

const IoTDashboard = () => {
  const [activeTab, setActiveTab] = useState('inventory');
  const [data, setData] = useState({
    Inventory: {
      door: {
        opened_at: "",
        closed_at: ""
      },
      stock: 0
    },
    Motion: {
      door: {
        opened_at: "",
        closed_at: ""
      }
    },
    Store: {
      products: 0
    }
  });

  // Check for zero items and show notifications
  useEffect(() => {
    const inventoryCount = Math.floor((data.Inventory?.stock) / 74) || 0;
    const storeCount = Math.floor((data.Store?.products) / 74) || 0;

    if (inventoryCount === 0) {
      Alert.alert(
        "",
        "The inventory count has reached 0 items!",
        [{ text: "OK" }]
      );
    }

    if (storeCount === 0) {
      Alert.alert(
        "",
        "The store products count has reached 0 items!",
        [{ text: "OK" }]
      );
    }
  }, [data.Inventory?.stock, data.Store?.products]);

  useEffect(() => {
    const dbRef = ref(db, '/');
    return onValue(dbRef, (snapshot) => {
      try {
        const newData = snapshot.val();
        console.log(newData);
        setData(newData);
      } catch (error) {
        console.log('Error fetching data:', error);
      }
    });
  }, []);

  const getDoorStatus = (door: any) => {
    if (!door) return "Unknown";
    const lastOpenedTime = new Date(door.opened_at);
    const lastClosedTime = new Date(door.closed_at);
    return lastOpenedTime > lastClosedTime ? "Open" : "Closed";
  };

  const renderDoorInfo = (doorData, title) => (
    <InfoCard
      title={`${title} Door Status`}
      items={[
        {
          label: "Status",
          value: getDoorStatus(doorData)
        },
        {
          label: "Last Opened",
          value: doorData?.opened_at
        },
        {
          label: "Last Closed",
          value: doorData?.closed_at
        }
      ]}
    />
  );

  const renderTabContent = () => {
    switch (activeTab) {
      case 'inventory':
        return (
          <View>
            <InfoCard
              title="Inventory Status"
              items={[
                { label: "Current Stock", value: Math.floor((data.Inventory?.stock) / 70) || 0 }
              ]}
            />
            {renderDoorInfo(data.Inventory?.door, "Inventory")}
          </View>
        );
      case 'motion':
        return renderDoorInfo(data.Motion?.door, "Motion");
      case 'store':
        return (
          <InfoCard
            title="Store Status"
            items={[
              { label: "Products", value: Math.floor((data.Store?.products) / 70) || 0 }
            ]}
          />
        );
      default:
        return null;
    }
  };

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView>
        <View style={styles.quickStats}>
          <QuickStatCard
            title="Inventory Stock"
            value={`${Math.floor((data.Inventory?.stock) / 70) || 0} items in stock `}
            subtitle={''}
            color="#2196F3"
          />
          <QuickStatCard
            title="Inventory Door"
            value={getDoorStatus(data.Inventory?.door)}
            subtitle={`Last opened: ${data.Inventory?.door?.opened_at}`}
            color="#4CAF50"
          />
          <QuickStatCard
            title="Motion Door"
            value={getDoorStatus(data.Motion?.door)}
            subtitle={`Last opened: ${data.Motion?.door?.opened_at}`}
            color="#FFC107"
          />
          <QuickStatCard
            title="Store Products"
            value={`${Math.floor((data.Store?.products) / 70) || 0} item in the store`}
            subtitle={''}
            color="#9C27B0"
          />
        </View>

        <View style={styles.tabContainer}>
          {['inventory', 'motion', 'store'].map((tab) => (
            <TouchableOpacity
              key={tab}
              style={[styles.tab, activeTab === tab && styles.activeTab]}
              onPress={() => setActiveTab(tab)}
            >
              <Text style={[styles.tabText, activeTab === tab && styles.activeTabText]}>
                {tab.charAt(0).toUpperCase() + tab.slice(1)}
              </Text>
            </TouchableOpacity>
          ))}
        </View>

        <View style={styles.tabContent}>
          {renderTabContent()}
        </View>
      </ScrollView>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: 40,
    backgroundColor: '#f5f5f5',
  },
  quickStats: {
    padding: 16,
    gap: 12,
  },
  tabContainer: {
    flexDirection: 'row',
    backgroundColor: 'white',
    paddingTop: 8,
    paddingHorizontal: 16,
    borderBottomWidth: 1,
    borderBottomColor: '#f0f0f0',
  },
  tab: {
    flex: 1,
    paddingVertical: 12,
    alignItems: 'center',
    marginHorizontal: 4,
  },
  activeTab: {
    borderBottomWidth: 2,
    borderBottomColor: '#2196F3',
  },
  tabText: {
    color: '#888',
    fontSize: 13,
    fontWeight: '500',
  },
  activeTabText: {
    color: '#2196F3',
    fontWeight: '600',
  },
  tabContent: {
    padding: 16,
  }
});

export default IoTDashboard;