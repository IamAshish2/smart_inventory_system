import { View, Text, StyleSheet } from 'react-native';

type InfoItem = {
    label: string;
    value: string | number;
}

type InfoCardProps = {
    title: string;
    items: InfoItem[];
}

const InfoCard = ({ title, items }: InfoCardProps) => (
    <View style={styles.infoCard}>
        <Text style={styles.sectionTitle}>{title}</Text>
        {items.map((item, index) => (
            <View 
                key={index} 
                style={[
                    styles.infoRow,
                    index === items.length - 1 && { borderBottomWidth: 0 }
                ]}
            >
                <Text style={styles.infoLabel}>{item.label}</Text>
                <Text style={styles.infoValue}>{item.value}</Text>
            </View>
        ))}
    </View>
);

export default InfoCard;

const styles = StyleSheet.create({
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
});
