// Fix for the QuickStatCard component - removed TypeScript optional parameter syntax
const QuickStatCard = ({ title, value, subtitle, color }) => (
  <View style={[styles.quickStatCard, { borderLeftColor: color }]}>
    <Text style={styles.cardTitle}>{title}</Text>
    <Text style={styles.cardValue}>{value}</Text>
    {subtitle && <Text style={styles.cardSubtitle}>{subtitle}</Text>}
  </View>
);

// The rest of the code remains the same as it was already correct, but here's the full working version
import { db } from '@/lib/firebase/firebase';
import { StatusBar } from 'expo-status-bar';
import { onValue, ref } from 'firebase/database';
import React, { useEffect, useState } from 'react';
import { View, Text, ScrollView, StyleSheet, TouchableOpacity, SafeAreaView, } from 'react-native';

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

  const getDoorStatus = (door) => {
    if (!door) return "Unknown";
    const lastOpenedTime = new Date(door.opened_at);
    const lastClosedTime = new Date(door.closed_at);
    return lastOpenedTime > lastClosedTime ? "Open" : "Closed";
  };

  const formatTime = (timestamp) => {
    if (!timestamp || timestamp.includes("1970")) return "Not available";
    const date = new Date(timestamp);
    return date.toLocaleTimeString('en-US', {
      hour: '2-digit',
      minute: '2-digit'
    });
  };

  const InfoCard = ({ title, items }) => (
    <View style={styles.infoCard}>
      <Text style={styles.sectionTitle}>{title}</Text>
      {items.map((item, index) => (
        <View key={index} style={styles.infoRow}>
          <Text style={styles.infoLabel}>{item.label}</Text>
          <Text style={styles.infoValue}>{item.value}</Text>
        </View>
      ))}
    </View>
  );

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
                { label: "Current Stock", value: data.Inventory?.stock || 0 }
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
              { label: "Products", value: data.Store?.products || 0 }
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
            value={`${data.Inventory?.stock || 0} items`}
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
            value={data.Store?.products || 0}
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
    marginTop: 20,
    backgroundColor: '#f5f5f5',
  },
  quickStats: {
    padding: 16,
    gap: 12,
  },
  quickStatCard: {
    backgroundColor: 'white',
    borderRadius: 12,
    padding: 16,
    width: '100%',
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 1 },
    shadowOpacity: 0.15,
    shadowRadius: 3,
    elevation: 3,
    borderLeftWidth: 4,
    marginBottom: 0,
  },
  cardTitle: {
    fontSize: 13,
    color: '#666',
    marginBottom: 8,
    textTransform: 'uppercase',
    letterSpacing: 0.5,
  },
  cardValue: {
    fontSize: 28,
    fontWeight: '600',
    marginBottom: 8,
    color: '#222',
  },
  cardSubtitle: {
    fontSize: 13,
    color: '#666',
    lineHeight: 18,
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
  },
  infoCard: {
    backgroundColor: 'white',
    borderRadius: 12,
    padding: 16,
    marginBottom: 16,
  },
  infoRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    paddingVertical: 12,
    paddingHorizontal: 4,
    borderBottomWidth: 1,
    borderBottomColor: '#f0f0f0',
  },
  infoLabel: {
    fontSize: 14,
    color: '#666',
    flex: 1,
  },
  infoValue: {
    fontSize: 15,
    fontWeight: '500',
    color: '#222',
    marginLeft: 16,
  },
  sectionTitle: {
    fontSize: 20,
    fontWeight: '600',
    marginBottom: 16,
    color: '#222',
    paddingHorizontal: 4,
  },
  storeMetrics: {
    gap: 16,
  },
});

export default IoTDashboard;